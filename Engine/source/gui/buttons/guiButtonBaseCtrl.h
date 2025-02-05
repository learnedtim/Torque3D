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

#ifndef _GUIBUTTONBASECTRL_H_
#define _GUIBUTTONBASECTRL_H_

#ifndef _GUICONTROL_H_
#include "gui/core/guiControl.h"
#endif


/// Base class for all button controls.  Subclasses are mostly for specific
/// rendering types.
///
class GuiButtonBaseCtrl : public GuiControl
{
public:

   typedef GuiControl Parent;

   enum ButtonType
   {
      ButtonTypePush,
      ButtonTypeCheck,
      ButtonTypeRadio,
   };

protected:

   StringTableEntry mButtonText;
   StringTableEntry mButtonTextID;
   bool mDepressed;
   bool mHighlighted;
   bool mStateOn;
   S32 mButtonType;
   S32 mRadioGroup;
   bool mUseMouseEvents;

   /// Point where left mouse button was pressed down.  Used to find when to start
   /// a mouse drag.
   Point2I mMouseDownPoint;

   ///
   bool mMouseDragged;

   /// @name Callbacks
   /// @{

   DECLARE_CALLBACK(void, onMouseDown, ());
   DECLARE_CALLBACK(void, onMouseUp, ());
   DECLARE_CALLBACK(void, onClick, ());
   DECLARE_CALLBACK(void, onRightClick, ());
   DECLARE_CALLBACK(void, onDoubleClick, ());
   DECLARE_CALLBACK(void, onMouseEnter, ());
   DECLARE_CALLBACK(void, onMouseLeave, ());
   DECLARE_CALLBACK(void, onMouseDragged, ());
   DECLARE_CALLBACK(void, onHighlighted, (bool));

   /// @}

public:

   GuiButtonBaseCtrl();
   bool onWake() override;

   DECLARE_CONOBJECT(GuiButtonBaseCtrl);
   DECLARE_CATEGORY("Gui Buttons");
   DECLARE_DESCRIPTION("A basic button control.");

   static void initPersistFields();

   void setText(const char* text);
   void setTextID(S32 id);
   void setTextID(const char* id);
   const char* getText();
   void setStateOn(bool bStateOn);
   bool getStateOn() const { return mStateOn; }

   void setDepressed(bool depressed) { mDepressed = depressed; }
   void resetState() { mDepressed = false; mHighlighted = false; }

   void setHighlighted(bool highlighted);
   bool isHighlighted() { return mHighlighted; }

   void acceleratorKeyPress(U32 index) override;
   void acceleratorKeyRelease(U32 index) override;

   void onMouseDown(const GuiEvent&) override;
   void onMouseUp(const GuiEvent&) override;
   void onMouseDragged(const GuiEvent& event) override;
   void onRightMouseUp(const GuiEvent&) override;

   void onMouseEnter(const GuiEvent&) override;
   void onMouseLeave(const GuiEvent&) override;

   bool onKeyDown(const GuiEvent& event) override;
   bool onKeyUp(const GuiEvent& event) override;

   void setScriptValue(const char* value) override;
   const char* getScriptValue() override;

   void onMessage(GuiControl*, S32 msg) override;
   void onAction() override;

   bool usesMouseEvents() const { return mUseMouseEvents; }
   void setUseMouseEvents(bool val) { mUseMouseEvents = val; }
};

typedef GuiButtonBaseCtrl::ButtonType GuiButtonType;
DefineEnumType(GuiButtonType);

#endif
