#include "ShaderData.hlsli"

// --- CORE FUNCTION ---
// Calculates the total displacement and the new normal for a vertex
// --- CORE FUNCTION ---
void CalculateGerstnerDisplacement(in float3 position, out float3 displacedPosition, out float3 normal)
{
    float3 displacement = float3(0.0f, 0.0f, 0.0f);
    
    // dP/dx starts at (1, 0, 0)
    float3 dPdx = float3(1.0f, 0.0f, 0.0f);
    // dP/dz starts at (0, 0, 1)
    float3 dPdz = float3(0.0f, 0.0f, 1.0f);

    for (int i = 0; i < 4; i++)
    {
        GerstnerWave wave = Waves[i];
        
        // 1. Pre-calculate Wave Parameters
        float k_mag = 2.0f * 3.14159f / wave.Wavelength;
        float omega = wave.Speed * k_mag;
        float2 k = wave.Direction * k_mag;
        
        float amplitude = wave.Steepness / k_mag; // A

        // 2. Calculate Phase
        float phase = dot(position.xz, k) + omega * Time;
        
        float sin_phase = sin(phase);
        float cos_phase = cos(phase);

        // 3. Calculate Displacement (D)
        float Dx_z = amplitude * cos_phase;
        float Dy = amplitude * sin_phase;
        
        displacement.x += wave.Direction.x * Dx_z;
        displacement.y += Dy;
        displacement.z += wave.Direction.y * Dx_z;

        // 4. Calculate Derivatives (for Normal Calculation)
        
        float kx = k.x;
        float kz = k.y;

        // Common terms: A * cos(phi) and A * sin(phi)
        float A_cos = amplitude * cos_phase;
        float A_sin = amplitude * sin_phase;

        // dP/dx ACCUMULATION (Tangents)
        // d(Dx)/dx = -A * Dx * kx * sin(phi)
        dPdx.x += -wave.Direction.x * kx * A_sin;
        // d(Dy)/dx = A * kx * cos(phi)
        dPdx.y += kx * A_cos;
        // d(Dz)/dx = -A * Dz * kx * sin(phi)
        dPdx.z += -wave.Direction.y * kx * A_sin;

        // dP/dz ACCUMULATION (Tangents)
        // d(Dx)/dz = -A * Dx * kz * sin(phi)
        dPdz.x += -wave.Direction.x * kz * A_sin;
        // d(Dy)/dz = A * kz * cos(phi)
        dPdz.y += kz * A_cos;
        // d(Dz)/dz = -A * Dz * kz * sin(phi)
        dPdz.z += -wave.Direction.y * kz * A_sin;
    }

    // 5. Final Position and Normal
    displacedPosition = position + displacement;

    // Normal is the cross product of the tangent vectors
    normal = normalize(cross(dPdz, dPdx));
}

// Entry point for the vertex shader - will be executed for each vertex
PixelInput main(VertexInput input)
{
    PixelInput pixel_input;
    
    float3 originalPosition = input.position.xyz;
    float3 finalPosition;
    float3 finalNormal;

    // 1. Calculate displaced position and World Space Normal
    CalculateGerstnerDisplacement(originalPosition, finalPosition, finalNormal);

    // 2. Transform to homogeneous clip space
    pixel_input.position = mul(float4(finalPosition, 1.0f), cModelViewProjection);

    // 3. CORRECT NORMAL TRANSFORMATION
    // We need the Normal Matrix: Transpose(Inverse(Model))
    // cModelInverse is M_inverse (untransposed in HLSL due to the transpose trick)
    // We must transpose it here to get (M_inverse)^T
    
    // Convert to a 3x3 matrix for normal transformation
    float3x3 cNormalMatrix = (float3x3) cModelInverse;

    // Apply the transpose: mul(finalNormal, transpose(cModelInverse))
    // Note: The mul() order is optimized for row vectors, but for a 3x3 matrix 
    // it's clearest to use the transpose() function explicitly.
    pixel_input.normal = mul(finalNormal, transpose(cNormalMatrix));
    
    // IMPORTANT: Normalize the normal again after transformation to handle floating point error
    pixel_input.normal = normalize(pixel_input.normal);

    // ... (texture coordinates unchanged) ...
    pixel_input.tex1 = mul(float4(input.tex, 0.0f, 1.0f), cTextureMatrix1).xy;
    pixel_input.tex2 = mul(float4(input.tex, 0.0f, 1.0f), cTextureMatrix2).xy;

    return pixel_input;
}