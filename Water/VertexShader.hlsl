#include "ShaderData.hlsli"

// Entry point for the vertex shader - will be executed for each vertex
PixelInput main(VertexInput input)
{
    // Wave parameters (can be passed in as constants or uniforms)
    float A = 0.1f; // Amplitude
    float wavelength = 1.0f; // Wavelength
    float frequency = 0.1f; // Frequency
    
    // Wave direction vector (must be normalized and on the XZ plane)
    // IMPORTANT: Make sure this vector is normalized, e.g., float3(1.0f, 0.0f, 0.0f) is normalized.
    float3 waveDirection = float3(1.0f, 0.0f, 0.0f);

    // Calculate wave parameters
    float k_mag = 2.0f * 3.14159f / wavelength; // Wave vector magnitude (k)
    float omega = 2.0f * 3.14159f * frequency; // Angular frequency (omega)
    
    // Calculate the wave vector components (kx, kz)
    float2 k = waveDirection.xz * k_mag;

    PixelInput pixel_input;
    
    // 1. Calculate the phase (dot(k, position.xz) + omega * time)
    // position.xz is the horizontal position (x, z)
    // The phase determines where the vertex is in the wave cycle at time 'time'
    float phase = dot(input.position.xz, k) + omega * time;

    // 2. Calculate the Gerstner displacement components
    float cos_phase = cos(phase);
    float sin_phase = sin(phase);
    
    // Horizontal displacement (X and Z)
    // Dx = (A * (kx/|k|) * cos(phase))
    // Dz = (A * (kz/|k|) * cos(phase))
    // We can simplify: (kx/|k|) = waveDirection.x and (kz/|k|) = waveDirection.z 
    // since |waveDirection| = 1.0f
    float dx = A * waveDirection.x * cos_phase;
    float dz = A * waveDirection.z * cos_phase;
    
    // Vertical displacement (Y) - uses sine for Gerstner waves
    // Dy = A * sin(phase)
    float dy = A * sin_phase;

    // 3. Apply the displacement to the vertex position
    float3 newPosition = input.position;
    newPosition.x += dx;
    newPosition.y += dy;
    newPosition.z += dz;

    // Transform to homogeneous clip space
    // Use the new, displaced position
    pixel_input.position = mul(float4(newPosition, 1.0f), cModelViewProjection);

    // [Optional] Update the normal vector. 
    // For a single Gerstner wave, the normal can be calculated more easily, 
    // but for multiple waves, you'd typically use a Jacobian matrix method.
    // For simplicity, we'll skip normal correction here.

    // Set the vertex texture coordinates (adjusting for displacement if needed, or keeping original)
    // Note: I'm leaving your original texture calculation untouched, 
    // but you should decide what to map to the texture coordinates.
    pixel_input.tex1 = mul(float4(input.tex, 0.0f, dy), cTextureMatrix1).xy;
    pixel_input.tex2 = mul(float4(input.tex, 0.0f, 1.0f), cTextureMatrix2).xy;

    return pixel_input;
}