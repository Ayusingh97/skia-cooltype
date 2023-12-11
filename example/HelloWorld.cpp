/*
* Copyright 2017 Google Inc.
*
* Use of this source code is governed by a BSD-style license that can be
* found in the LICENSE file.
*/

#include "example/HelloWorld.h"

#include "include/core/SkCanvas.h"
#include "include/core/SkColor.h"
#include "include/core/SkFont.h"
#include "include/core/SkFontTypes.h"
#include "include/core/SkGraphics.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPoint.h"
#include "include/core/SkRect.h"
#include "include/core/SkShader.h"
#include "include/core/SkString.h"
#include "include/core/SkSurface.h"
#include "include/core/SkTileMode.h"
#include "include/effects/SkGradientShader.h"
#include "tools/window/DisplayParams.h"

#include <string.h>
#include "SkFontHost_CoolType.h"

#include "BIBHost.h"
#include "BIBContainer.h"
#include "BIBStringAtom.h"
#include "BIBMemoryBuffer.h"
#include "BIBInit.h"
#include "BIBVer.h"
#include "BRVNumericTypes.h"
#include "BIBDataStore.h"
#include "BIBFileDataStore.h"
#include "BIB.h"
#include "CT.h"
#include "ACEInit.h"
#include "AGMInit.h"

#include <iostream>

using namespace sk_app;
using skwindow::DisplayParams;

Application* Application::Create(int argc, char** argv, void* platformData) {
    return new HelloWorld(argc, argv, platformData);
}

HelloWorld::HelloWorld(int argc, char** argv, void* platformData)
#if defined(SK_GL)
        : fBackendType(Window::kNativeGL_BackendType),
#elif defined(SK_VULKAN)
        : fBackendType(Window::kVulkan_BackendType),
#else
        : fBackendType(Window::kRaster_BackendType),
#endif
        fRotationAngle(0) {
    SkGraphics::Init();

    fWindow = Window::CreateNativeWindow(platformData);
    fWindow->setRequestedDisplayParams(DisplayParams());

    // register callbacks
    fWindow->pushLayer(this);

    fWindow->attach(fBackendType);
}

HelloWorld::~HelloWorld() {
    fWindow->detach();
    delete fWindow;
}

void HelloWorld::updateTitle() {
    if (!fWindow) {
        return;
    }

    SkString title("Hello World ");
    if (Window::kRaster_BackendType == fBackendType) {
        title.append("Raster");
    } else {
#if defined(SK_GL)
        title.append("GL");
#elif defined(SK_VULKAN)
        title.append("Vulkan");
#elif defined(SK_DAWN)
        title.append("Dawn");
#else
        title.append("Unknown GPU backend");
#endif
    }

    fWindow->setTitle(title.c_str());
}

void HelloWorld::onBackendCreated() {
    this->updateTitle();
    fWindow->show();
    fWindow->inval();
}

void bravo_init()
{
    BIBGetAddressProc fBIBGetProc = BIBHostInit(0, 0, 0, 0,0,"bibleak");
    std::cout<<"Is BIB Initialized: " <<std::endl;
    if (fBIBGetProc)
        std::cout<<"YES! Continue execution... " << std::endl;
    else
    {
        std::cout<<"NO!! Returning from here... "<< fBIBGetProc <<std::endl;
        return;
    }
    
    //CoolType Initialise
    CTFontConfig    fInitFlags = kCTIncludeExtendedFonts | kCTPreferSystemFonts;
    CTScript        fLocScript = kCTRomanScript;
    CTFileConfig config = { kCTInit_v2,0,0,0,0,0,0,0,0 };
    config.unicodeDirectory = "typesupport/unicode/";
    config.fntNamesDBFile = "typesupport/FntNames.db";

    BRVBool32 ret = CTInit(fBIBGetProc, fInitFlags, fLocScript, &config);

    if (ret)
    {
        std::cout<<"CoolType Init: Success!" << std::endl;
    }
    else
    {
        std::cout<<"CoolType Init: Failed!" << std::endl;
        return;
    }

}

void HelloWorld::onPaint(SkSurface* surface) {
    
    bravo_init();
    
    auto canvas = surface->getCanvas();

    // Clear background
    canvas->clear(SK_ColorWHITE);

    std::string fontPath = "/Users/ayushisingh/Downloads/nitni_fonts/MinionPro-Regular.otf"; // Replace with the path to your custom font file
    sk_sp<SkTypeface> typeface = SkTypeface_CoolType::MakeFromFile(fontPath.c_str());
    //SkTypeface_CoolType::MakeFromFile(fontPath.c_str());
        
        if (!typeface) {
            std::cout << "Failed to load the custom font file." << std::endl;
            return;
        }
        // Set the text position
        SkPoint textPosition = SkPoint::Make(50, 1000); // Set the position of the text
        // Create an SkPaint for the text
        SkPaint paint;
        paint.setAntiAlias(true);
        paint.setColor(SK_ColorBLACK);
        // Set the font for the text
        SkFont font(nullptr, 50); // Default typeface with size 24
        font.setTypeface(typeface); // Specify a different typeface if desired
        // Draw the text on the canvas
        std::string text = "HELLO";
        canvas->drawSimpleText(text.c_str(), text.length(), SkTextEncoding::kUTF8, textPosition.x(), textPosition.y(), font, paint);
    
	canvas->restore();
}

void HelloWorld::onIdle() {
    // Just re-paint continuously
    fWindow->inval();
}

bool HelloWorld::onChar(SkUnichar c, skui::ModifierKey modifiers) {
    if (' ' == c) {
        if (Window::kRaster_BackendType == fBackendType) {
#if defined(SK_GL)
            fBackendType = Window::kNativeGL_BackendType;
#elif defined(SK_VULKAN)
            fBackendType = Window::kVulkan_BackendType;
#else
            SkDebugf("No GPU backend configured\n");
            return true;
#endif
        } else {
            fBackendType = Window::kRaster_BackendType;
        }
        fWindow->detach();
        fWindow->attach(fBackendType);
    }
    return true;
}
