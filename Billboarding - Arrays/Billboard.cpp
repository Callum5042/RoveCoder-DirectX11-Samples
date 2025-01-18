#include "Billboard.h"
#include "Renderer.h"

#include <vector>
#include <string>
#include <filesystem>

#include "../External/WICTextureLoader.h"

Billboard::Billboard(Renderer* renderer) : m_Renderer(renderer)
{
}

void Billboard::Create()
{
	CreateVertexBuffer();
	LoadTextureArray();
}

void Billboard::CreateVertexBuffer()
{
	ID3D11Device* device = m_Renderer->GetDevice();

	// Set vertex data
	m_Vertices =
	{
		{ 0.0f, 1.0f, 0.0f, 2.0f, 2.0f },
		{ -3.0f, 1.0f, 0.0f, 2.0f, 2.0f },
		{ 3.0f, 1.0f, 0.0f, 2.0f, 2.0f },
	};

	// Create index buffer
	D3D11_BUFFER_DESC vertex_buffer_desc = {};
	vertex_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	vertex_buffer_desc.ByteWidth = static_cast<UINT>(sizeof(BillboardVertex) * m_Vertices.size());
	vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vertex_subdata = {};
	vertex_subdata.pSysMem = m_Vertices.data();

	DX::Check(device->CreateBuffer(&vertex_buffer_desc, &vertex_subdata, m_VertexBuffer.ReleaseAndGetAddressOf()));
}

void Billboard::LoadTextureArray()
{
    // Define the texture paths
    std::vector<std::wstring> paths = { L"oaktree_billboard.png", L"ginko_billboard.png", L"maple_billboard.png", L"willow_billboard.png" };

    // Check if files exist
    for (const auto& path : paths)
    {
        if (!std::filesystem::exists(path))
        {
            std::wstring error = L"Could not load file: " + path;
            MessageBox(NULL, error.c_str(), L"Error", MB_OK);
            return;
        }
    }

    ID3D11Device* device = m_Renderer->GetDevice();
    ID3D11DeviceContext* context = m_Renderer->GetDeviceContext();

    // Load the first texture to determine dimensions and format
    ComPtr<ID3D11Resource> tempResource = nullptr;
    ComPtr<ID3D11Texture2D> tempTexture = nullptr;
    DirectX::CreateWICTextureFromFile(device, context, paths[0].c_str(), tempResource.ReleaseAndGetAddressOf(), nullptr);
    DX::Check(tempResource.As(&tempTexture));

    D3D11_TEXTURE2D_DESC textureDesc;
    tempTexture->GetDesc(&textureDesc);

    // Update the description for a Texture2DArray
    textureDesc.ArraySize = static_cast<UINT>(paths.size());
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.MiscFlags = 0;

    // Create the Texture2DArray
    ComPtr<ID3D11Texture2D> textureArray;
    DX::Check(device->CreateTexture2D(&textureDesc, nullptr, textureArray.ReleaseAndGetAddressOf()));

    // Load each texture into the array
    for (UINT i = 0; i < paths.size(); ++i)
    {
        ComPtr<ID3D11Resource> srcResource = nullptr;
        ComPtr<ID3D11Texture2D> srcTexture = nullptr;

        // Load the texture file
        DX::Check(DirectX::CreateWICTextureFromFile(device, context, paths[i].c_str(), srcResource.ReleaseAndGetAddressOf(), nullptr));
        DX::Check(srcResource.As(&srcTexture));

        // Copy the data into the corresponding slice of the Texture2DArray
        context->CopySubresourceRegion(
            textureArray.Get(),
            D3D11CalcSubresource(0, i, textureDesc.MipLevels), // Destination subresource (mip 0, slice i)
            0, 0, 0, // Destination coordinates
            srcTexture.Get(),
            0, // Source subresource (mip 0)
            nullptr // Copy the whole texture
        );
    }

    // Create the Shader Resource View for the Texture2DArray
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = textureDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
    srvDesc.Texture2DArray.MostDetailedMip = 0;
    srvDesc.Texture2DArray.MipLevels = textureDesc.MipLevels;
    srvDesc.Texture2DArray.FirstArraySlice = 0;
    srvDesc.Texture2DArray.ArraySize = static_cast<UINT>(paths.size());

    DX::Check(device->CreateShaderResourceView(textureArray.Get(), &srvDesc, m_DiffuseTexture.ReleaseAndGetAddressOf()));
}


void Billboard::Render()
{
	ID3D11DeviceContext* context = m_Renderer->GetDeviceContext();

	// We need the stride and offset for the vertex
	UINT vertex_stride = sizeof(BillboardVertex);
	UINT vertex_offset = 0;

	// Bind the vertex buffer to the Input Assembler
	context->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &vertex_stride, &vertex_offset);

	// Bind the geometry topology to the Input Assembler
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	// Bind texture to the pixel shader
	context->PSSetShaderResources(0, 1, m_DiffuseTexture.GetAddressOf());

	// Render geometry
	context->Draw(static_cast<UINT>(m_Vertices.size()), 0);
}
