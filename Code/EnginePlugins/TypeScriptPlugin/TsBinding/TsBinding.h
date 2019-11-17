#pragma once

#include <TypeScriptPlugin/TypeScriptPluginDLL.h>

#include <Core/ResourceManager/ResourceHandle.h>
#include <Core/Scripting/DuktapeContext.h>
#include <Core/World/Declarations.h>
#include <Core/World/World.h>
#include <Foundation/Containers/HashTable.h>
#include <Foundation/Math/Declarations.h>
#include <Foundation/Types/Uuid.h>
#include <TypeScriptPlugin/Resources/ScriptCompendiumResource.h>
#include <TypeScriptPlugin/Transpiler/Transpiler.h>

class ezWorld;
class ezTypeScriptComponent;
struct ezMsgTypeScriptMsgProxy;

using ezJavaScriptResourceHandle = ezTypedResourceHandle<class ezJavaScriptResource>;

enum ezTypeScriptBindingIndexProperty
{
  ComponentHandle,
  GameObjectHandle
};

class EZ_TYPESCRIPTPLUGIN_DLL ezTypeScriptBinding
{
public:
  struct TsMessageHandler
  {
    const ezRTTI* m_pMessageType = nullptr;
    ezUInt32 m_uiMessageTypeNameHash = 0;
    ezString m_sHandlerFunc;
  };

  struct TsComponentInfo
  {
    ezString m_sComponentTypeName;
    ezHybridArray<TsMessageHandler, 4> m_MessageHandlers;
  };

  using TsComponentTypeInfo = ezMap<ezUuid, TsComponentInfo>::ConstIterator;

  /// \name Basics
  ///@{

public:
  ezTypeScriptBinding();
  ~ezTypeScriptBinding();

  ezResult Initialize(ezWorld& world);
  ezResult LoadComponent(const ezUuid& typeGuid, TsComponentTypeInfo& out_TypeInfo);

  void RegisterMessageHandlersForComponentType(const char* szComponent, const ezUuid& componentType);

  EZ_ALWAYS_INLINE ezDuktapeContext& GetDukTapeContext() { return m_Duk; }
  EZ_ALWAYS_INLINE duk_context* GetDukContext() { return m_Duk.GetContext(); }

private:
  static void GetTsName(const ezRTTI* pRtti, ezStringBuilder& out_sName);

  ezDuktapeContext m_Duk;
  bool m_bInitialized = false;
  ezMap<ezUuid, bool> m_LoadedComponents;
  ezScriptCompendiumResourceHandle m_hScriptCompendium;

  ///@}
  /// \name Typescript Code Generation
  ///@{
public:
  static ezResult SetupProjectCode();

private:
  static void GenerateComponentCode(ezStringBuilder& out_Code, const ezRTTI* pRtti);
  static void GenerateExposedFunctionsCode(ezStringBuilder& out_Code, const ezRTTI* pRtti);
  static void GeneratePropertiesCode(ezStringBuilder& out_Code, const ezRTTI* pRtti);
  static void GenerateAllComponentsCode(ezStringBuilder& out_Code);
  static void GenerateComponentsFile(const char* szFile);
  static void InjectComponentImportExport(const char* szFile, const char* szComponentFile);
  static void InjectMessageImportExport(const char* szFile, const char* szComponentFile);

  ///@}
  /// \name Function Binding
  ///@{

public:
  struct FunctionBinding
  {
    ezAbstractFunctionProperty* m_pFunc = nullptr;
  };

  static const FunctionBinding* FindFunctionBinding(ezUInt32 uiFunctionHash);

private:
  static ezUInt32 ComputeFunctionBindingHash(const ezRTTI* pType, ezAbstractFunctionProperty* pFunc);
  static void SetupRttiFunctionBindings();
  static const char* TsType(const ezRTTI* pRtti);
  static int __CPP_Binding_RegisterMessageHandler(duk_context* pDuk);

  static ezHashTable<ezUInt32, FunctionBinding> s_BoundFunctions;


  ///@}
  /// \name Property Binding
  ///@{

public:
  struct PropertyBinding
  {
    ezAbstractMemberProperty* m_pMember = nullptr;
  };

  static const PropertyBinding* FindPropertyBinding(ezUInt32 uiHash);

private:
  static ezUInt32 ComputePropertyBindingHash(const ezRTTI* pType, ezAbstractMemberProperty* pMember);
  static void SetupRttiPropertyBindings();

  static ezHashTable<ezUInt32, PropertyBinding> s_BoundProperties;

  ///@}
  /// \name Message Binding
  ///@{

public:
  ezUniquePtr<ezMessage> MessageFromParameter(duk_context* pDuk, ezInt32 iObjIdx, ezTime delay);
  static void DukPutMessage(duk_context* pDuk, const ezMessage& msg);

  bool DeliverMessage(const TsComponentTypeInfo& typeInfo, ezTypeScriptComponent* pComponent, ezMessage& msg);
  bool DeliverTsMessage(const TsComponentTypeInfo& typeInfo, ezTypeScriptComponent* pComponent, const ezMsgTypeScriptMsgProxy& msg);

private:
  static void GenerateMessagesFile(const char* szFile);
  static void GenerateAllMessagesCode(ezStringBuilder& out_Code);
  static void GenerateMessageCode(ezStringBuilder& out_Code, const ezRTTI* pRtti);
  static void GenerateMessagePropertiesCode(ezStringBuilder& out_Code, const ezRTTI* pRtti);

  ezUuid m_CurrentTsMsgHandlerRegistrator;
  ezMap<ezUuid, TsComponentInfo> m_TsComponentTypes;


  ///@}
  /// \name Modules
  ///@{
private:
  static int DukSearchModule(duk_context* pDuk);

  ///@}
  /// \name Initialization
  ///@{
private:
  ezResult Init_RequireModules();
  ezResult Init_Log();
  ezResult Init_Time();
  ezResult Init_GameObject();
  ezResult Init_Component();
  ezResult Init_World();
  ezResult Init_Clock();
  ezResult Init_Random();
  ezResult Init_FunctionBinding();
  ezResult Init_PropertyBinding();
  ezResult Init_Debug();
  ezResult Init_Physics();


  ///@}
  /// \name ezWorld
  ///@{
public:
  static ezWorld* RetrieveWorld(duk_context* pDuk);
  static ezTypeScriptBinding* RetrieveBinding(duk_context* pDuk);

private:
  void StoreWorld(ezWorld* pWorld);

  ezWorld* m_pWorld = nullptr;
  static ezHashTable<duk_context*, ezWorld*> s_DukToWorld;

  ///@}
  /// \name ezGameObject
  ///@{
public:
  static ezGameObjectHandle RetrieveGameObjectHandle(duk_context* pDuk, ezInt32 iObjIdx = 0 /* use 0, if the game object is passed in as the 'this' object (first parameter) */);
  static ezGameObject* ExpectGameObject(duk_context* pDuk, ezInt32 iObjIdx = 0 /* use 0, if the game object is passed in as the 'this' object (first parameter) */);
  bool DukPutGameObject(const ezGameObjectHandle& hObject);
  void DukPutGameObject(const ezGameObject* pObject);

  ///@}
  /// \name Components
  ///@{
public:
  void DukPutComponentObject(const ezComponentHandle& hComponent);
  void DukPutComponentObject(ezComponent* pComponent);
  void DeleteTsComponent(const ezComponentHandle& hCppComponent);
  static ezComponentHandle RetrieveComponentHandle(duk_context* pDuk, ezInt32 iObjIdx = 0 /* use 0, if the component is passed in as the 'this' object (first parameter) */);

  template <typename ComponentType>
  static ComponentType* ExpectComponent(duk_context* pDuk, ezInt32 iObjIdx = 0 /* use 0, if the game object is passed in as the 'this' object (first parameter) */);

  ///@}
  /// \name Math
  ///@{

  static void PushVec2(duk_context* pDuk, const ezVec2& value);
  static void SetVec2(duk_context* pDuk, ezInt32 iObjIdx, const ezVec2& value);
  static void SetVec2Property(duk_context* pDuk, const char* szPropertyName, ezInt32 iObjIdx, const ezVec2& value);
  static ezVec2 GetVec2(duk_context* pDuk, ezInt32 iObjIdx, const ezVec2& fallback = ezVec2::ZeroVector());
  static ezVec2 GetVec2Property(duk_context* pDuk, const char* szPropertyName, ezInt32 iObjIdx, const ezVec2& fallback = ezVec2::ZeroVector());

  static void PushVec3(duk_context* pDuk, const ezVec3& value);
  static void SetVec3(duk_context* pDuk, ezInt32 iObjIdx, const ezVec3& value);
  static void SetVec3Property(duk_context* pDuk, const char* szPropertyName, ezInt32 iObjIdx, const ezVec3& value);
  static ezVec3 GetVec3(duk_context* pDuk, ezInt32 iObjIdx, const ezVec3& fallback = ezVec3::ZeroVector());
  static ezVec3 GetVec3Property(duk_context* pDuk, const char* szPropertyName, ezInt32 iObjIdx, const ezVec3& fallback = ezVec3::ZeroVector());

  static void PushMat3(duk_context* pDuk, const ezMat3& value);
  static void SetMat3(duk_context* pDuk, ezInt32 iObjIdx, const ezMat3& value);
  static void SetMat3Property(duk_context* pDuk, const char* szPropertyName, ezInt32 iObjIdx, const ezMat3& value);
  static ezMat3 GetMat3(duk_context* pDuk, ezInt32 iObjIdx, const ezMat3& fallback = ezMat3::IdentityMatrix());
  static ezMat3 GetMat3Property(duk_context* pDuk, const char* szPropertyName, ezInt32 iObjIdx, const ezMat3& fallback = ezMat3::IdentityMatrix());

  static void PushMat4(duk_context* pDuk, const ezMat4& value);
  static void SetMat4(duk_context* pDuk, ezInt32 iObjIdx, const ezMat4& value);
  static void SetMat4Property(duk_context* pDuk, const char* szPropertyName, ezInt32 iObjIdx, const ezMat4& value);
  static ezMat4 GetMat4(duk_context* pDuk, ezInt32 iObjIdx, const ezMat4& fallback = ezMat4::IdentityMatrix());
  static ezMat4 GetMat4Property(duk_context* pDuk, const char* szPropertyName, ezInt32 iObjIdx, const ezMat4& fallback = ezMat4::IdentityMatrix());

  static void PushQuat(duk_context* pDuk, const ezQuat& value);
  static void SetQuat(duk_context* pDuk, ezInt32 iObjIdx, const ezQuat& value);
  static void SetQuatProperty(duk_context* pDuk, const char* szPropertyName, ezInt32 iObjIdx, const ezQuat& value);
  static ezQuat GetQuat(duk_context* pDuk, ezInt32 iObjIdx, ezQuat fallback = ezQuat::IdentityQuaternion());
  static ezQuat GetQuatProperty(duk_context* pDuk, const char* szPropertyName, ezInt32 iObjIdx, ezQuat fallback = ezQuat::IdentityQuaternion());

  static void PushColor(duk_context* pDuk, const ezColor& value);
  static void SetColor(duk_context* pDuk, ezInt32 iObjIdx, const ezColor& value);
  static void SetColorProperty(duk_context* pDuk, const char* szPropertyName, ezInt32 iObjIdx, const ezColor& value);
  static ezColor GetColor(duk_context* pDuk, ezInt32 iObjIdx, const ezColor& fallback = ezColor::White);
  static ezColor GetColorProperty(duk_context* pDuk, const char* szPropertyName, ezInt32 iObjIdx, const ezColor& fallback = ezColor::White);

  static void PushTransform(duk_context* pDuk, const ezTransform& value);
  static void SetTransform(duk_context* pDuk, ezInt32 iObjIdx, const ezTransform& value);
  static void SetTransformProperty(duk_context* pDuk, const char* szPropertyName, ezInt32 iObjIdx, const ezTransform& value);
  static ezTransform GetTransform(duk_context* pDuk, ezInt32 iObjIdx, const ezTransform& fallback = ezTransform::IdentityTransform());
  static ezTransform GetTransformProperty(duk_context* pDuk, const char* szPropertyName, ezInt32 iObjIdx, const ezTransform& fallback = ezTransform::IdentityTransform());

  static void PushVariant(duk_context* pDuk, const ezVariant& value);
  static void SetVariantProperty(duk_context* pDuk, const char* szPropertyName, ezInt32 iObjIdx, const ezVariant& value);
  static ezVariant GetVariant(duk_context* pDuk, ezInt32 iObjIdx, ezVariant::Type::Enum type);
  static ezVariant GetVariantProperty(duk_context* pDuk, const char* szPropertyName, ezInt32 iObjIdx, ezVariant::Type::Enum type);

  ///@}
  /// \name C++ Object Registration
  ///@{
public:
  bool RegisterGameObject(ezGameObjectHandle handle, ezUInt32& out_uiStashIdx);
  ezResult RegisterComponent(const char* szTypeName, ezComponentHandle handle, ezUInt32& out_uiStashIdx);

  /// \brief Removes dead GameObject and Component references from the DukTape stash.
  void CleanupStash(ezUInt32 uiNumIterations);

private:
  static void StoreReferenceInStash(duk_context* pDuk, ezUInt32 uiStashIdx);
  static bool DukPushStashObject(duk_context* pDuk, ezUInt32 uiStashIdx);

  static constexpr ezUInt32 c_uiMaxMsgStash = 512;
  static constexpr ezUInt32 c_uiFirstStashMsgIdx = 512;
  static constexpr ezUInt32 c_uiLastStashMsgIdx = c_uiFirstStashMsgIdx + c_uiFirstStashMsgIdx;
  static constexpr ezUInt32 c_uiFirstStashObjIdx = c_uiLastStashMsgIdx;
  ezUInt32 m_uiNextStashMsgIdx = c_uiFirstStashMsgIdx;
  ezUInt32 m_uiNextStashObjIdx = c_uiFirstStashObjIdx;
  ezMap<ezGameObjectHandle, ezUInt32> m_GameObjectToStashIdx;
  ezMap<ezComponentHandle, ezUInt32> m_ComponentToStashIdx;
  ezDeque<ezUInt32> m_FreeStashObjIdx;
  ezMap<ezGameObjectHandle, ezUInt32>::Iterator m_LastCleanupObj;
  ezMap<ezComponentHandle, ezUInt32>::Iterator m_LastCleanupComp;
  ezDynamicArray<ezTime> m_StashedMsgDelivery;

  ///@}
};

template <typename ComponentType>
ComponentType* ezTypeScriptBinding::ExpectComponent(duk_context* pDuk, ezInt32 iObjIdx /*= 0 */)
{
  ezComponentHandle hOwnHandle = ezTypeScriptBinding::RetrieveComponentHandle(pDuk, iObjIdx);

  ComponentType* pComponent = nullptr;
  ezWorld* pWorld = ezTypeScriptBinding::RetrieveWorld(pDuk);
  EZ_VERIFY(pWorld->TryGetComponent(hOwnHandle, pComponent), "Invalid component parameter");

  return pComponent;
}
