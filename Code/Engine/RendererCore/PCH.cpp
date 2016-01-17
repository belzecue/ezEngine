#include <RendererCore/PCH.h>
#include <Foundation/PCH.h>

EZ_STATICLINK_LIBRARY(RendererCore)
{
  if (bReturn)
    return;

  EZ_STATICLINK_REFERENCE(RendererCore_ConstantBuffers_ConstantBufferResource);
  EZ_STATICLINK_REFERENCE(RendererCore_Material_Implementation_MaterialResource);
  EZ_STATICLINK_REFERENCE(RendererCore_Lights_Implementation_LightComponent);
  EZ_STATICLINK_REFERENCE(RendererCore_Lights_Implementation_PointLightComponent);
  EZ_STATICLINK_REFERENCE(RendererCore_Lights_Implementation_SpotLightComponent);
  EZ_STATICLINK_REFERENCE(RendererCore_Lights_Implementation_DirectionalLightComponent);
  EZ_STATICLINK_REFERENCE(RendererCore_Lights_Implementation_LightGatheringRenderer);
  EZ_STATICLINK_REFERENCE(RendererCore_Meshes_Implementation_MeshBufferResource);
  EZ_STATICLINK_REFERENCE(RendererCore_Meshes_Implementation_MeshComponent);
  EZ_STATICLINK_REFERENCE(RendererCore_Meshes_Implementation_MeshRenderer);
  EZ_STATICLINK_REFERENCE(RendererCore_Meshes_Implementation_MeshResource);
  EZ_STATICLINK_REFERENCE(RendererCore_Pipeline_Implementation_RenderHelper);
  EZ_STATICLINK_REFERENCE(RendererCore_Pipeline_Implementation_RenderPipeline);
  EZ_STATICLINK_REFERENCE(RendererCore_Pipeline_Implementation_RenderPipelinePass);
  EZ_STATICLINK_REFERENCE(RendererCore_Pipeline_Implementation_View);
  EZ_STATICLINK_REFERENCE(RendererCore_ShaderCompiler_Implementation_PermutationGenerator);
  EZ_STATICLINK_REFERENCE(RendererCore_ShaderCompiler_Implementation_ShaderCompiler);
  EZ_STATICLINK_REFERENCE(RendererCore_ShaderCompiler_Implementation_ShaderManager);
  EZ_STATICLINK_REFERENCE(RendererCore_Shader_Helper);
  EZ_STATICLINK_REFERENCE(RendererCore_Shader_ShaderPermutationBinary);
  EZ_STATICLINK_REFERENCE(RendererCore_Shader_ShaderPermutationResource);
  EZ_STATICLINK_REFERENCE(RendererCore_Shader_ShaderResource);
  EZ_STATICLINK_REFERENCE(RendererCore_Shader_ShaderStageBinary);
  EZ_STATICLINK_REFERENCE(RendererCore_Shader_Startup);
  EZ_STATICLINK_REFERENCE(RendererCore_Textures_TextureResource);
}



