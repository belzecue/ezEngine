#include <ParticlePlugin/PCH.h>
#include <ParticlePlugin/Type/Sprite/SpriteRenderer.h>
#include <RendererFoundation/Device/Device.h>
#include <RendererCore/RenderContext/RenderContext.h>
#include <RendererFoundation/Context/Context.h>
#include <RendererCore/Pipeline/RenderDataBatch.h>
#include <RendererCore/Shader/ShaderResource.h>
#include <Foundation/Types/ScopeExit.h>

#include <RendererCore/../../../Data/Base/Shaders/Particles/ParticleSystemConstants.h>

EZ_BEGIN_DYNAMIC_REFLECTED_TYPE(ezParticleSpriteRenderData, 1, ezRTTINoAllocator)
EZ_END_DYNAMIC_REFLECTED_TYPE

EZ_BEGIN_DYNAMIC_REFLECTED_TYPE(ezParticleSpriteRenderer, 1, ezRTTIDefaultAllocator<ezParticleSpriteRenderer>);
EZ_END_DYNAMIC_REFLECTED_TYPE

ezParticleSpriteRenderData::ezParticleSpriteRenderData()
{
  m_uiNumParticles = 0;
}


ezParticleSpriteRenderer::ezParticleSpriteRenderer()
{
}

ezParticleSpriteRenderer::~ezParticleSpriteRenderer()
{
  if (!m_hDataBuffer.IsInvalidated())
  {
    ezGALDevice::GetDefaultDevice()->DestroyBuffer(m_hDataBuffer);
    m_hDataBuffer.Invalidate();
  }
}

void ezParticleSpriteRenderer::GetSupportedRenderDataTypes(ezHybridArray<const ezRTTI*, 8>& types)
{
  types.PushBack(ezGetStaticRTTI<ezParticleSpriteRenderData>());
}

void ezParticleSpriteRenderer::CreateDataBuffer()
{
  if (m_hDataBuffer.IsInvalidated())
  {
    ezGALBufferCreationDescription desc;
    desc.m_uiStructSize = sizeof(ezSpriteParticleData);
    desc.m_uiTotalSize = s_uiMaxBufferSize;
    desc.m_BufferType = ezGALBufferType::Generic;
    desc.m_bUseAsStructuredBuffer = true;
    desc.m_bAllowShaderResourceView = true;
    desc.m_ResourceAccess.m_bImmutable = false;

    m_hDataBuffer = ezGALDevice::GetDefaultDevice()->CreateBuffer(desc);
  }
}

void ezParticleSpriteRenderer::RenderBatch(const ezRenderViewContext& renderViewContext, ezRenderPipelinePass* pPass, const ezRenderDataBatch& batch)
{
  ezGALDevice* pDevice = ezGALDevice::GetDefaultDevice();
  ezGALContext* pGALContext = renderViewContext.m_pRenderContext->GetGALContext();

  /// \brief This pattern looks like it is inefficient. Should it use the GPU pool instead somehow?
  // prepare the constant buffer
  ezConstantBufferStorage<ezParticleSystemConstants>* pConstantBuffer;
  ezConstantBufferStorageHandle hConstantBuffer = ezRenderContext::CreateConstantBufferStorage(pConstantBuffer);
  EZ_SCOPE_EXIT(ezRenderContext::DeleteConstantBufferStorage(hConstantBuffer));
  renderViewContext.m_pRenderContext->BindConstantBuffer("ezParticleSystemConstants", hConstantBuffer);

  // Bind the Sprite particle shader
  {
    if (!m_hShader.IsValid())
    {
      m_hShader = ezResourceManager::LoadResource<ezShaderResource>("Shaders/Particles/Sprite.ezShader");
    }

    renderViewContext.m_pRenderContext->BindShader(m_hShader);
  }

  const ezUInt32 uiParticlesPerBatch(s_uiMaxBufferSize / sizeof(ezSpriteParticleData));

  // make sure our structured buffer is allocated and bound
  {
    CreateDataBuffer();
    renderViewContext.m_pRenderContext->BindMeshBuffer(ezGALBufferHandle(), ezGALBufferHandle(), nullptr, ezGALPrimitiveTopology::Triangles, uiParticlesPerBatch * 2);
    renderViewContext.m_pRenderContext->BindBuffer(ezGALShaderStage::VertexShader, "particleData", pDevice->GetDefaultResourceView(m_hDataBuffer));
  }

  // now render all particle effects of type Sprite
  for (auto it = batch.GetIterator<ezParticleSpriteRenderData>(0, batch.GetCount()); it.IsValid(); ++it)
  {
    const ezParticleSpriteRenderData* pRenderData = it;
    ezUInt32 uiNumParticles = pRenderData->m_uiNumParticles;

    const ezSpriteParticleData* pParticleData = pRenderData->m_GpuData->m_Content.GetData();

    renderViewContext.m_pRenderContext->BindTexture(ezGALShaderStage::PixelShader, "ParticleTexture", pRenderData->m_hTexture);

    // fill the constant buffer
    {
      ezParticleSystemConstants& cb = pConstantBuffer->GetDataForWriting();
      cb.ObjectToWorldMatrix = pRenderData->m_GlobalTransform.GetAsMat4();
    }

    while (uiNumParticles > 0)
    {
      // upload this batch of particle data
      const ezUInt32 uiNumParticlesInBatch = ezMath::Min<ezUInt32>(uiNumParticles, uiParticlesPerBatch);
      pGALContext->UpdateBuffer(m_hDataBuffer, 0, ezMakeArrayPtr(pParticleData, uiNumParticlesInBatch).ToByteArray());

      // do one drawcall
      renderViewContext.m_pRenderContext->DrawMeshBuffer(uiNumParticlesInBatch * 2);

      uiNumParticles -= uiNumParticlesInBatch;
      pParticleData += uiNumParticlesInBatch;
    }
  }
}
