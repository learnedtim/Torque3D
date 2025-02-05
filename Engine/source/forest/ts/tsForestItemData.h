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

#ifndef _TSFORESTITEMDATA_H_
#define _TSFORESTITEMDATA_H_

#ifndef _FORESTITEM_H_
#include "forest/forestItem.h"
#endif
#ifndef _TSSHAPE_H_
#include "ts/tsShape.h"
#endif
#ifndef _RESOURCEMANAGER_H_
#include "core/resourceManager.h"
#endif


class TSShapeInstance;
class TSLastDetail;


class TSForestItemData : public ForestItemData 
{
protected:

   typedef ForestItemData Parent;

   bool mIsClientObject;

   // This is setup during forest creation.
   mutable TSShapeInstance *mShapeInstance;

   Vector<S32> mCollisionDetails;
   Vector<S32> mLOSDetails;
   
   TSShapeInstance* _getShapeInstance() const;

   void _loadShape();

   void _onResourceChanged( const Torque::Path &path );

   void _checkLastDetail();

   void _updateCollisionDetails();

   // ForestItemData
   void _preload() override { _loadShape(); }

public:
  
   DECLARE_CONOBJECT(TSForestItemData);
   TSForestItemData();
   virtual ~TSForestItemData();
   
   bool preload( bool server, String &errorBuffer ) override;   
   void onRemove() override;
   bool onAdd() override;

   void inspectPostApply() override;

   TSLastDetail* getLastDetail() const;

   // JCF: need access to this to build convex(s) in ForestConvex
   TSShapeInstance* getShapeInstance() const { return _getShapeInstance(); }

   const Vector<S32>& getCollisionDetails() const { return mCollisionDetails; }
   const Vector<S32>& getLOSDetails() const { return mLOSDetails; }

   // ForestItemData
   const Box3F& getObjBox() const override { return mShape ? mShape->mBounds : Box3F::Zero; }
   bool render( TSRenderState *rdata, const ForestItem& item ) const override;
   ForestCellBatch* allocateBatch() const override;
   bool canBillboard( const SceneRenderState *state, const ForestItem &item, F32 distToCamera ) const override;
   bool buildPolyList( const ForestItem& item, AbstractPolyList *polyList, const Box3F *box ) const { return false; }
};

#endif // _TSFORESTITEMDATA_H_
