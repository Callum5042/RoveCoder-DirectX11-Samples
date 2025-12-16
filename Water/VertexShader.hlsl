#include "ShaderData.hlsli"

// --- CORE FUNCTION ---
// Calculates the total displacement and the new normal for a vertex
void CalculateGerstnerDisplacement(in float3 position, out float3 displacedPosition, out float3 normal)
{
    // Initialize displacement and tangent vectors
    float3 displacement = float3(0.0f, 0.0f, 0.0f);
    
    // Initialize the derivative vectors (used for normal calculation)
    // dP/dx is the tangent vector in the X direction
    float3 dPdx = float3(1.0f, 0.0f, 0.0f);
    // dP/dz is the tangent vector in the Z direction
    float3 dPdz = float3(0.0f, 0.0f, 1.0f);

    // Loop through all waves
    for (int i = 0; i < 4; i++)
    {
        GerstnerWave wave = Waves[i];
        
        // 1. Pre-calculate Wave Parameters
        float k_mag = 2.0f * 3.14159f / wave.Wavelength; // Wave vector magnitude |k|
        float omega = wave.Speed * k_mag; // Angular frequency (omega = Speed * |k|)
        float2 k = wave.Direction * k_mag; // Wave vector k = |k| * Direction (kx, kz)
        
        // The Steepness 'Q' is often scaled by the inverse of the wave vector magnitude: Q/|k|
        // A common practice for stable waves is to use a fixed Q (e.g., 1.0) and set A = Q / |k|
        float amplitude = wave.Steepness / k_mag; // Calculate A based on Q and |k|
        
        // 2. Calculate Phase (phi = k . x_xz + omega * t)
        float phase = dot(position.xz, k) + omega * Time;
        
        float sin_phase = sin(phase);
        float cos_phase = cos(phase);

        // 3. Calculate Displacement (D)
        
        // Horizontal displacement (X and Z) - uses cosine
        float Dx_z = amplitude * cos_phase;
        
        // Vertical displacement (Y) - uses sine
        float Dy = amplitude * sin_phase;
        
        // Accumulate total displacement
        displacement.x += wave.Direction.x * Dx_z;
        displacement.y += Dy;
        displacement.z += wave.Direction.y * Dx_z; // Note: wave.Direction.y is used for the Z component

        // 4. Calculate Derivatives (for Normal Calculation)
        // Derivatives are calculated by finding the partial derivatives of the position function
        // P(x,z) = (x + Dx, y + Dy, z + Dz) with respect to x and z.
        // d(sin(phi))/dx = cos(phi) * (d(phi)/dx) = cos(phi) * kx
        // d(cos(phi))/dx = -sin(phi) * (d(phi)/dx) = -sin(phi) * kx
        
        // Derivative of Phase w.r.t x and z:
        // d(phi)/dx = kx
        // d(phi)/dz = kz
        float kx = k.x;
        float kz = k.y;

        // d(D)/dx = (A * Q * kx * -sin(phi), A * kx * cos(phi), A * Q * kz * -sin(phi))
        
        // This term is used in the derivatives calculation
        float K_cos = amplitude * k_mag * cos_phase;
        float K_sin = amplitude * k_mag * sin_phase;

        // dP/dx = d(x+Dx)/dx , d(y+Dy)/dx , d(z+Dz)/dx
        // dP/dx is the tangent vector in the X direction
        dPdx.x += -wave.Direction.x * kx * K_sin; // Simplified version of d(Dx)/dx
        dPdx.y += kx * K_cos; // Simplified version of d(Dy)/dx
        dPdx.z += -wave.Direction.y * kx * K_sin; // Simplified version of d(Dz)/dx

        // dP/dz = d(x+Dx)/dz , d(y+Dy)/dz , d(z+Dz)/dz
        // dP/dz is the tangent vector in the Z direction
        dPdz.x += -wave.Direction.x * kz * K_sin; // Simplified version of d(Dx)/dz
        dPdz.y += kz * K_cos; // Simplified version of d(Dy)/dz
        dPdz.z += -wave.Direction.y * kz * K_sin; // Simplified version of d(Dz)/dz
    }

    // 5. Final Position and Normal
    
    // Add the total displacement to the original position
    displacedPosition = position + displacement;

    // The normal vector is the cross product of the two tangent vectors (dP/dz x dP/dx)
    // The order determines the direction (which side of the surface is 'up')
    normal = normalize(cross(dPdz, dPdx));
}

// Entry point for the vertex shader - will be executed for each vertex
PixelInput main(VertexInput input)
{
    PixelInput pixel_input;
    
    float3 originalPosition = input.position.xyz;
    float3 finalPosition;
    float3 finalNormal;

    // Call the function to calculate displacement and normal
    CalculateGerstnerDisplacement(originalPosition, finalPosition, finalNormal);

    // Transform to homogeneous clip space
    // Use the new, displaced position
    pixel_input.position = mul(float4(finalPosition, 1.0f), cModelViewProjection);


    pixel_input.tex1 = mul(float4(input.tex, 0.0f, 1.0f), cTextureMatrix1).xy;
    pixel_input.tex2 = mul(float4(input.tex, 0.0f, 1.0f), cTextureMatrix2).xy;

    return pixel_input;
}