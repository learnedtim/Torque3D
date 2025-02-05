//-----------------------------------------------------------------------------
// Copyright (c) 2012 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#ifndef _ADVANCEDLIGHTBINMANAGER_H_
#define _ADVANCEDLIGHTBINMANAGER_H_

#ifndef _TEXTARGETBIN_MGR_H_
#include "renderInstance/renderTexTargetBinManager.h"
#endif
#ifndef _TVECTOR_H_
#include "core/util/tVector.h"
#endif
#ifndef _LIGHTINFO_H_
#include "lighting/lightInfo.h"
#endif
#ifndef _MATHUTIL_FRUSTUM_H_
#include "math/util/frustum.h"
#endif
#ifndef _MATINSTANCE_H_
#include "materials/matInstance.h"
#endif
#ifndef _SHADOW_COMMON_H_
#include "lighting/shadowMap/shadowCommon.h"
#endif


class AdvancedLightManager;
class ShadowMapManager;
class LightShadowMap;
class AdvancedLightBufferConditioner;
class LightMapParams;


class LightMatInstance : public MatInstance
{
   typedef MatInstance Parent;
protected:
   MaterialParameterHandle *mLightMapParamsSC;
   bool mInternalPass;

   enum
   {
      DynamicLight = 0,
      SunLight,
      StaticLightNonLMGeometry,
      StaticLightLMGeometry,
      NUM_LIT_STATES,
      Base = 0,
      Reflecting = 1
   };
   GFXStateBlockRef mLitState[NUM_LIT_STATES][2];

public:
   LightMatInstance(Material &mat) : Parent(mat), mLightMapParamsSC(NULL), mInternalPass(false),  mSpecialLight(false) {}

   bool init( const FeatureSet &features, const GFXVertexFormat *vertexFormat ) override;
   bool setupPass( SceneRenderState *state, const SceneData &sgData ) override;

   bool mSpecialLight;
};

class AdvancedLightBinManager : public RenderBinManager
{
   typedef RenderBinManager Parent;

public:

   // Light info Render Inst Type
   static const RenderInstType RIT_LightInfo;
   
   // registered buffer name
   static const String smBufferName;

   NamedTexTargetRef mDiffuseLightingTarget;
   GFXTexHandle      mDiffuseLightingTex;

   GFXTextureTargetRef mLightingTargetRef;

   /// The shadow filter mode to use on shadowed light materials.
   static ShadowFilterMode smShadowFilterMode;

   /// Used to toggle the PSSM debug rendering mode.
   static bool smPSSMDebugRender;

   /// Set by the SSAO post effect to tell the vector
   /// light to compile in the SSAO mask.
   static bool smUseSSAOMask;

   //Used to toggle the visualization of diffuse and specular light contribution
   static bool smDiffuseLightViz;
   static bool smSpecularLightViz;
   static bool smDetailLightingViz;

   static S32 smMaximumNumOfLights;
   static bool smUseLightFade;
   static F32 smLightFadeEnd;
   static F32 smLightFadeStart;

   static bool smAllowLocalLightShadows;

   // Used for console init
   AdvancedLightBinManager( AdvancedLightManager *lm = NULL, 
                            ShadowMapManager *sm = NULL,
                            GFXFormat lightBufferFormat = GFXFormatR8G8B8A8 );
   virtual ~AdvancedLightBinManager();

   // ConsoleObject
   static void consoleInit();

   // RenderBinManager
   void render(SceneRenderState *) override;
   void clear() override {}
   void sort() override {}

   // Add a light to the bins
   void addLight( LightInfo *light );

   // Clear all lights from the bins
   void clearAllLights();

   virtual bool setTargetSize(const Point2I &newTargetSize);

   /// Scores the registered lights for sorting/ordering purposes
   void _scoreLights(const MatrixF& cameraTrans);

   // ConsoleObject interface
   DECLARE_CONOBJECT(AdvancedLightBinManager);

   bool MRTLightmapsDuringDeferred() const { return mMRTLightmapsDuringDeferred; }
   void MRTLightmapsDuringDeferred(bool val);

   bool _updateTargets();

   typedef Signal<void(SceneRenderState *, AdvancedLightBinManager *)> RenderSignal;
   static RenderSignal &getRenderSignal();

   AdvancedLightManager *getManager() { return mLightManager; }

protected:

   /// Frees all the currently allocated light materials.
   void _deleteLightMaterials();

   // Track a light material and associated data
   struct LightMaterialInfo
   {
      LightMatInstance *matInstance;

      // { zNear, zFar, 1/zNear, 1/zFar }
      MaterialParameterHandle *zNearFarInvNearFar;

      // Far frustum plane (World Space)
      MaterialParameterHandle *farPlane;

      // Far frustum plane (View Space)
      MaterialParameterHandle *vsFarPlane;

      // -dot( farPlane, eyePos )
      MaterialParameterHandle *negFarPlaneDotEye;

      // Light Parameters
      MaterialParameterHandle *lightPosition;
      MaterialParameterHandle *lightDirection;
      MaterialParameterHandle *lightColor;
      MaterialParameterHandle *lightBrightness;
      MaterialParameterHandle *lightRange;
      MaterialParameterHandle *lightInvSqrRange;
      MaterialParameterHandle *lightAmbient;
      MaterialParameterHandle *lightSpotParams;

      LightMaterialInfo(   const String &matName, 
                           const GFXVertexFormat *vertexFormat,
                           const Vector<GFXShaderMacro> &macros = Vector<GFXShaderMacro>() );

      virtual ~LightMaterialInfo();


      void setViewParameters( const F32 zNear, 
                              const F32 zFar, 
                              const Point3F &eyePos, 
                              const PlaneF &farPlane,
                              const PlaneF &_vsFarPlane );

      void setLightParameters( const LightInfo *light, const SceneRenderState* renderState );
   };

protected:

   struct LightBinEntry
   {
      LightInfo* lightInfo;
      LightShadowMap* shadowMap;
      LightMaterialInfo* lightMaterial;
      GFXPrimitiveBuffer* primBuffer;
      GFXVertexBuffer* vertBuffer;
      U32 numPrims;
   };

   Vector<LightBinEntry> mLightBin;
   typedef Vector<LightBinEntry>::iterator LightBinIterator;

   bool mMRTLightmapsDuringDeferred;

   /// Used in setupSGData to set the object transform.
   MatrixF mLightMat;

   U32 mNumLightsCulled;
   AdvancedLightManager *mLightManager;
   ShadowMapManager *mShadowManager;

   static const String smLightMatNames[LightInfo::Count];

   static const String smShadowTypeMacro[ShadowType_Count];

   static const GFXVertexFormat* smLightMatVertex[LightInfo::Count];

   typedef CompoundKey4<LightInfo::Type,ShadowType,bool, bool> LightMatKey;

   typedef HashTable<LightMatKey,LightMaterialInfo*> LightMatTable;

   /// The fixed table of light material info.
   LightMatTable mLightMaterials;

   LightMaterialInfo* _getLightMaterial( LightInfo::Type lightType, ShadowType shadowType, bool useCookieTex = false, bool isPhotometric = false );

   ///
   void _onShadowFilterChanged();

   typedef GFXVertexPNTT FarFrustumQuadVert; 
   GFXVertexBufferHandle<FarFrustumQuadVert> mFarFrustumQuadVerts;


   //void _createMaterials();

   void _setupPerFrameParameters( const SceneRenderState *state );

   void setupSGData( SceneData &data, const SceneRenderState* state, LightInfo *light );

   static S32 QSORT_CALLBACK _lightScoreCmp(const LightBinEntry* a, const  LightBinEntry* b);
};

#endif // _ADVANCEDLIGHTBINMANAGER_H_
