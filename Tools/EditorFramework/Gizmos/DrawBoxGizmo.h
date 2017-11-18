#pragma once

#include <ToolsFoundation/Basics.h>
#include <EditorEngineProcessFramework/Gizmos/GizmoHandle.h>
#include <EditorFramework/Gizmos/GizmoBase.h>
#include <QPoint>

class EZ_EDITORFRAMEWORK_DLL ezDrawBoxGizmo : public ezGizmo
{
  EZ_ADD_DYNAMIC_REFLECTION(ezDrawBoxGizmo, ezGizmo);

public:
  ezDrawBoxGizmo();

protected:
  virtual void DoFocusLost(bool bCancel) override;

  virtual ezEditorInput DoMousePressEvent(QMouseEvent* e) override;
  virtual ezEditorInput DoMouseReleaseEvent(QMouseEvent* e) override;
  virtual ezEditorInput DoMouseMoveEvent(QMouseEvent* e) override;
  virtual ezEditorInput DoKeyPressEvent(QKeyEvent* e) override;
  virtual ezEditorInput DoKeyReleaseEvent(QKeyEvent* e) override;

  virtual void OnSetOwner(ezQtEngineDocumentWindow* pOwnerWindow, ezQtEngineViewWidget* pOwnerView) override;
  virtual void OnVisibleChanged(bool bVisible) override;
  virtual void OnTransformationChanged(const ezTransform& transform) override;

private:
  enum class ManipulateMode
  {
    None,
    DrawBase,
    DrawHeight,
  };

  void SwitchMode(bool bCancel);
  void UpdateBox();

  ManipulateMode m_ManipulateMode;
  ezEngineGizmoHandle m_Box;

  ezInt32 m_iHeightChange = 0;
  ezVec2I32 m_LastMousePos;
  ezVec3 m_vCurrentPosition;
  ezVec3 m_vFirstCorner;
  ezVec3 m_vSecondCorner;
  float m_fBoxHeight = 0.5f;
  float m_fOriginalBoxHeight = 0.5f;
};
