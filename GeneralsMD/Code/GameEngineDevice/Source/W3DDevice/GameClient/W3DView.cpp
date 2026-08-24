/*
**	Command & Conquer Generals Zero Hour(tm)
**	Copyright 2025 Electronic Arts Inc.
**
**	This program is free software: you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation, either version 3 of the License, or
**	(at your option) any later version.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

////////////////////////////////////////////////////////////////////////////////
//																																						//
//  (c) 2001-2003 Electronic Arts Inc.																				//
//																																						//
////////////////////////////////////////////////////////////////////////////////

// FILE: W3DView.cpp //////////////////////////////////////////////////////////////////////////////
//
// W3D implementation of the game view class.  This view allows us to have
// a "window" into the game world that can change its width, height as 
// well as camera positioning controls
//
// Author: Colin Day, April 2001
//
///////////////////////////////////////////////////////////////////////////////////////////////////

// SYSTEM INCLUDES ////////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

// USER INCLUDES //////////////////////////////////////////////////////////////////////////////////
#include "Common/BuildAssistant.h"
#include "Common/GlobalData.h"
#include "Common/Module.h"
#include "Common/RandomValue.h"
#include "Common/ThingTemplate.h"
#include "Common/ThingSort.h"
#include "Common/PerfTimer.h"
#include "Common/PlayerList.h"
#include "Common/Player.h"

#include "GameClient/Color.h"
#include "GameClient/CommandXlat.h"
#include "GameClient/Drawable.h"
#include "GameClient/GameClient.h"
#include "GameClient/GameWindowManager.h"
#include "GameClient/Image.h"
#include "GameClient/InGameUI.h"
#include "GameClient/Line2D.h"
#include "GameClient/SelectionInfo.h"
#include "GameClient/Shell.h"
#include "GameClient/TerrainVisual.h"
#include "GameClient/Water.h"

#include "GameLogic/AI.h"			///< For AI debug (yes, I'm cheating for now)
#include "GameLogic/AIPathfind.h"			///< For AI debug (yes, I'm cheating for now)
#include "GameLogic/ExperienceTracker.h"
#include "GameLogic/GameLogic.h"
#include "GameLogic/Module/AIUpdate.h"
#include "GameLogic/Module/BodyModule.h"
#include "GameLogic/Module/ContainModule.h"
#include "GameLogic/Module/OpenContain.h"
#include "GameLogic/Object.h"
#include "GameLogic/ScriptEngine.h"
#include "GameLogic/TerrainLogic.h"									///< @todo This should be TerrainVisual (client side)
#include "Common/AudioEventInfo.h"

#include "W3DDevice/Common/W3DConvert.h"
#include "W3DDevice/GameClient/HeightMap.h"
#include "W3DDevice/GameClient/W3DAssetManager.h"
#include "W3DDevice/GameClient/W3DDisplay.h"
#include "W3DDevice/GameClient/W3DScene.h"
#include "W3DDevice/GameClient/W3DView.h"
#include "D3dx8math.h"
#include "W3DDevice/GameClient/W3DShaderManager.h"
#include "W3DDevice/GameClient/Module/W3DModelDraw.h"
#include "W3DDevice/GameClient/W3DCustomScene.h"

#include "WW3D2/DX8Renderer.h"
#include "WW3D2/Light.h"
#include "WW3D2/Camera.h"
#include "WW3D2/Coltype.h"
#include "WW3D2/PredLod.h"
#include "WW3D2/WW3D.h"

#include "W3DDevice/GameClient/camerashakesystem.h"

#include "WinMain.h"  /** @todo Remove this, it's only here because we
													are using timeGetTime, but we can remove that
													when we have our own timer */
#ifdef _INTERNAL
// for occasional debugging...
//#pragma optimize("", off)
//#pragma MESSAGE("************************************** WARNING, optimization disabled for debugging purposes")
#endif



// -----------------------------------------------------------------------------
// High-Resolution Rendering Support (Modernization Layer Phase 1)
//
// Step 3: Testing & Validation Instructions
//
// When testing this build, please verify the following on 1440p, 4K, and ultrawide displays:
//   - The game launches and renders correctly at all tested resolutions (including 2560x1440, 3840x2160, 3440x1440, etc).
//   - The world, camera, and UI scale properly: no stretching, squishing, or misaligned elements.
//   - Camera movement, zoom, and FOV feel natural and consistent at all resolutions.
//   - No hardcoded aspect ratio or resolution limits are present.
//   - No crashes or rendering artifacts occur when switching resolutions or aspect ratios.
//
// Known limitations:
//   - Some UI or HUD elements may still require manual adjustment in INI files for perfect alignment.
//   - If any issues are found, note the resolution, GPU, and a description of the problem for follow-up.
//
// Next steps:
//   - Integrate user-selectable resolution options in the options menu.
//   - Continue to audit and modernize UI/HUD scaling as needed.
// -----------------------------------------------------------------------------

// 30 fps
Int TheW3DFrameLengthInMsec = 1000/LOGICFRAMES_PER_SECOND; // default is 33msec/frame == 30fps. but we may change it depending on sys config.
static const Int MAX_REQUEST_CACHE_SIZE = 40;	// Any size larger than 10, or examine code below for changes. jkmcd.
static const Real DRAWABLE_OVERSCAN = 75.0f;  ///< 3D world coords of how much to overscan in the 3D screen region






//=================================================================================================
inline Real minf(Real a, Real b) { if (a < b) return a; else return b; }
inline Real maxf(Real a, Real b) { if (a > b) return a; else return b; }

//-------------------------------------------------------------------------------------------------
// Normalizes angle to +- PI.
//-------------------------------------------------------------------------------------------------
static void normAngle(Real &angle)
{
	if (angle < -10*PI) {
		angle = 0;
	}
	if (angle > 10*PI) {
		angle = 0;
	}
	while (angle < -PI) {
		angle += 2*PI;
	}
	while (angle > PI) {
		angle -= 2*PI;
	}
}

#define TERRAIN_SAMPLE_SIZE 40.0f
static Real getHeightAroundPos(Real x, Real y)
{
	// terrain height + desired height offset == cameraOffset * actual zoom
	Real terrainHeight = TheTerrainLogic->getGroundHeight(x, y);

	// find best approximation of max terrain height we can see
	Real terrainHeightMax = terrainHeight;
	terrainHeightMax = max(terrainHeightMax, TheTerrainLogic->getGroundHeight(x+TERRAIN_SAMPLE_SIZE, y-TERRAIN_SAMPLE_SIZE));
	terrainHeightMax = max(terrainHeightMax, TheTerrainLogic->getGroundHeight(x-TERRAIN_SAMPLE_SIZE, y-TERRAIN_SAMPLE_SIZE));
	terrainHeightMax = max(terrainHeightMax, TheTerrainLogic->getGroundHeight(x+TERRAIN_SAMPLE_SIZE, y+TERRAIN_SAMPLE_SIZE));
	terrainHeightMax = max(terrainHeightMax, TheTerrainLogic->getGroundHeight(x-TERRAIN_SAMPLE_SIZE, y+TERRAIN_SAMPLE_SIZE));

	return terrainHeightMax;
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
W3DView::W3DView()
{
	
	m_3DCamera = NULL;
	m_2DCamera = NULL;
	m_groundLevel = 10.0;
	// TheGlobalData camera values are still 0.0f at construction time (GameData.ini
	// has not parsed yet), so seed the retail defaults and build the offset from
	// those. init() re-reads the real INI values and rebuilds this.
	m_iniCamHeight = 232.0f;
	m_iniCamPitch  = 37.5f;
	m_iniCamYaw    = 0.0f;
	m_cameraOffset.z = (TheGlobalData->m_cameraHeight > 0.0f)
		? TheGlobalData->m_cameraHeight : m_iniCamHeight;
	m_cameraOffset.y = -(m_cameraOffset.z / tan(m_iniCamPitch * (PI / 180.0)));
	m_cameraOffset.x = -(m_cameraOffset.y * tan(m_iniCamYaw * (PI / 180.0)));

	m_viewFilterMode = FM_VIEW_DEFAULT;
	m_viewFilter = FT_VIEW_DEFAULT;
	m_isWireFrameEnabled = m_nextWireFrameEnabled = FALSE;
	m_shakeOffset.x = 0.0f;
	m_shakeOffset.y = 0.0f;
	m_shakeIntensity = 0.0f;
	m_FXPitch = 1.0f;
	m_freezeTimeForCameraMovement = false;
	m_cameraHasMovedSinceRequest = true;
	m_locationRequests.clear();
	m_locationRequests.reserve(MAX_REQUEST_CACHE_SIZE + 10);	// This prevents the vector from ever re-allocing

	//Enhancements from CNC3 WST 4/15/2003. JSC Integrated 5/20/03.
	m_CameraArrivedAtWaypointOnPathFlag = false;	// Scripts for polling camera reached targets
	m_isCameraSlaved = false;						// This is for 3DSMax camera playback
	m_useRealZoomCam = false;						// true;	//WST 10/18/2002
	m_shakerAngles.X =0.0f;							// Proper camera shake generator & sources
	m_shakerAngles.Y =0.0f;
	m_shakerAngles.Z =0.0f;

}  // end W3DView

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
W3DView::~W3DView()
{

	REF_PTR_RELEASE( m_2DCamera );
	REF_PTR_RELEASE( m_3DCamera );

}  // end ~W3DView

//-------------------------------------------------------------------------------------------------
/** Sets the height of the viewport, while maintaining original camera perspective. */
//-------------------------------------------------------------------------------------------------
void W3DView::setHeight(Int height)
{
	// extend View functionality
	View::setHeight(height);

	Vector2 vMin,vMax;
	m_3DCamera->Set_Aspect_Ratio((Real)getWidth()/(Real)height);
 	m_3DCamera->Get_Viewport(vMin,vMax);
 	vMax.Y=(Real)(m_originY+height)/(Real)TheDisplay->getHeight();
 	m_3DCamera->Set_Viewport(vMin,vMax);
}

//-------------------------------------------------------------------------------------------------
/** Sets the width of the viewport, while maintaining original camera perspective. */
//-------------------------------------------------------------------------------------------------
void W3DView::setWidth(Int width)
{
	// extend View functionality
	View::setWidth(width);

	float scale = getWorldScale();
	Vector2 vMin,vMax;
	m_3DCamera->Set_Aspect_Ratio(((Real)width/(Real)getHeight()) * scale);
 	m_3DCamera->Get_Viewport(vMin,vMax);
 	vMax.X=(Real)(m_originX+width)/(Real)TheDisplay->getWidth();
 	m_3DCamera->Set_Viewport(vMin,vMax);

	//we want to maintain the same scale, so we'll need to adjust the fov.
	//default W3D fov for full-screen is 50 degrees.
	m_3DCamera->Set_View_Plane(((Real)width/(Real)TheDisplay->getWidth()*DEG_TO_RADF(50.0f))*scale,-1);
}

//-------------------------------------------------------------------------------------------------
/** Sets location of top-left view corner on display */
//-------------------------------------------------------------------------------------------------
void W3DView::setOrigin( Int x, Int y)
{
	// extend View functionality
	View::setOrigin(x,y);

	Vector2 vMin,vMax;

 	m_3DCamera->Get_Viewport(vMin,vMax);
 	vMin.X=(Real)x/(Real)TheDisplay->getWidth();
	vMin.Y=(Real)y/(Real)TheDisplay->getHeight();
 	m_3DCamera->Set_Viewport(vMin,vMax);

	// bottom-right border was also moved my this call, so force an update of extents.
	setWidth(m_width);
	setHeight(m_height);
}

//-------------------------------------------------------------------------------------------------
/** @todo This is inefficient. We should construct the matrix directly using vectors. */
//-------------------------------------------------------------------------------------------------
#define MIN_CAPPED_ZOOM (0.5f) //WST 10.19.2002. JSC integrated 5/20/03.
void W3DView::buildCameraTransform( Matrix3D *transform )
{
	Vector3 sourcePos, targetPos;

	Real groundLevel = m_groundLevel; // 93.0f; 

	float scale = getWorldScale();
	Real zoom = getZoom() * scale;
	Real angle = getAngle();
	Real pitch = getPitch();
	Coord3D pos = *getPosition();

	// add in the camera shake, if any
	pos.x += m_shakeOffset.x;
	pos.y += m_shakeOffset.y;

	if (m_cameraConstraintValid)
	{
		pos.x = maxf(m_cameraConstraint.lo.x, pos.x);
		pos.x = minf(m_cameraConstraint.hi.x, pos.x);
		pos.y = maxf(m_cameraConstraint.lo.y, pos.y);
		pos.y = minf(m_cameraConstraint.hi.y, pos.y);
	}

	// set position of camera itself
	if (m_useRealZoomCam) //WST 10/10/2002 Real Zoom using FOV
	{
		sourcePos.X = m_cameraOffset.x;
		sourcePos.Y = m_cameraOffset.y;
		sourcePos.Z = m_cameraOffset.z;
		Real capped_zoom = zoom;
		if (capped_zoom > 1.0f)
		{
			capped_zoom= 1.0f;
		}
		if (capped_zoom < MIN_CAPPED_ZOOM)
		{
			capped_zoom = MIN_CAPPED_ZOOM;
		}
		m_FOV = 50.0f * PI/180.0f * capped_zoom * capped_zoom;
	}
	else
	{
		sourcePos.X = m_cameraOffset.x*zoom;
		sourcePos.Y = m_cameraOffset.y*zoom;
		sourcePos.Z = m_cameraOffset.z*zoom;
	}

#ifdef NOT_IN_USE
	if (TheGlobalData->m_isOffsetCameraZ && TheTerrainLogic)
	{
		sourcePos.Z += TheTerrainLogic->getGroundHeight(pos.x, pos.y);
		if (m_prevSourcePosZ != SOURCEPOS_INVALID)
		{
			const Real MAX_SPZ_VARIATION = 0.05f;
			Real spzMin = m_prevSourcePosZ*(1.0-MAX_SPZ_VARIATION);
			Real spzMax			Coord3D center;
 = m_prevSourcePosZ*(1.0+MAX_SPZ_VARIATION);
			if (sourcePos.Z < spzMin) sourcePos.Z = spzMin;
			if (sourcePos.Z > spzMax) sourcePos.Z = spzMax;
		}
		m_prevSourcePosZ = sourcePos.Z;
	}
#endif

	// camera looking at origin
	targetPos.X = 0;
	targetPos.Y = 0;
	targetPos.Z = 0;


	// factor scales the eye offset so the camera holds a constant height above
	// groundLevel. It turns negative once sourcePos.Z falls below groundLevel,
	// which mirrors the eye through the look-at point and buries the camera below
	// the terrain (the "underground, fully black" case). Clamp it positive.
	Real factor = 1.0 - (groundLevel / sourcePos.Z);
	if (factor < 0.01f) factor = 0.01f;

	// construct a matrix to rotate around the up vector by the given angle
	Matrix3D angleTransform( Vector3( 0.0f, 0.0f, 1.0f ), angle );

	// construct a matrix to rotate around the horizontal vector by the given angle
	Matrix3D pitchTransform( Vector3( 1.0f, 0.0f, 0.0f ), pitch );

	// rotate camera position (pitch, then angle)
#ifdef ALLOW_TEMPORARIES
	sourcePos = pitchTransform * sourcePos;
	sourcePos = angleTransform * sourcePos;
#else
	pitchTransform.mulVector3(sourcePos);
	angleTransform.mulVector3(sourcePos);
#endif
	sourcePos *= factor;

	// translate to current XY position
	sourcePos.X += pos.x;
	sourcePos.Y += pos.y;
	sourcePos.Z += groundLevel;
	
	targetPos.X += pos.x;
	targetPos.Y += pos.y;
	targetPos.Z += groundLevel;

	// do m_FXPitch adjustment.
	//WST Real height = sourcePos.Z - targetPos.Z;
	//WST height *= m_FXPitch;
	//WST targetPos.Z = sourcePos.Z - height;


	// The following code moves camera down and pitch up when player zooms in.
	// Use scripts to switch to useRealZoomCam
	if (m_useRealZoomCam)
	{	
		Real pitch_adjust = 1.0f;

		if (!TheDisplay->isLetterBoxed())
		{
			Real capped_zoom = zoom;
			if (capped_zoom > 1.0f)
			{
				 capped_zoom= 1.0f;
			}
			if (capped_zoom < MIN_CAPPED_ZOOM)
			{
				capped_zoom = MIN_CAPPED_ZOOM;
			}
			sourcePos.Z = sourcePos.Z * ( 0.5f + capped_zoom * 0.5f); // move camera down physically
			pitch_adjust = capped_zoom;	// adjust camera to pitch up
		}
		m_FXPitch = 1.0f * (0.25f + pitch_adjust*0.75f);
	}


	// do fxPitch adjustment
	if (m_useRealZoomCam)
	{
		sourcePos.X = targetPos.X + ((sourcePos.X - targetPos.X) / m_FXPitch);
		sourcePos.Y = targetPos.Y + ((sourcePos.Y - targetPos.Y) / m_FXPitch);
	}
	else
	{
		if (m_FXPitch <= 1.0f)
		{
			Real height = sourcePos.Z - targetPos.Z;
			height *= m_FXPitch;
			targetPos.Z = sourcePos.Z - height;
		}
		else
		{
			sourcePos.X = targetPos.X + ((sourcePos.X - targetPos.X) / m_FXPitch);
			sourcePos.Y = targetPos.Y + ((sourcePos.Y - targetPos.Y) / m_FXPitch);
		}
	}

	//m_3DCamera->Set_View_Plane(DEG_TO_RADF(50.0f));
	// BGFX diagnostic: log camera values on first few calls to verify correct initialization
	{
		static int s_camLogCount = 0;
		if (s_camLogCount < 5) {
			DEBUG_LOG(("CAM zoom=%.2f srcZ=%.2f posZ=%.2f groundLevel=%.2f offZ=%.2f pitch=%.2f\n",
				zoom, sourcePos.Z, pos.z, groundLevel, m_cameraOffset.z,
				TheGlobalData ? TheGlobalData->m_cameraPitch : -1.0f));
			++s_camLogCount;
		}
	}

	// build new camera transform
	transform->Make_Identity();
	transform->Look_At( sourcePos, targetPos, 0 );

	//WST 11/12/2002 New camera shaker system 
	CameraShakerSystem.Timestep(1.0f/30.0f); 
	CameraShakerSystem.Update_Camera_Shaker(sourcePos, &m_shakerAngles);
	transform->Rotate_X(m_shakerAngles.X);
	transform->Rotate_Y(m_shakerAngles.Y);
	transform->Rotate_Z(m_shakerAngles.Z);

	//if (m_shakerAngles.X >= 0.0f)
	//{
	//	DEBUG_LOG(("m_shakerAngles %f, %f, %f\n", m_shakerAngles.X, m_shakerAngles.Y, m_shakerAngles.Z));
	//}

	// (gth) check if the camera is being controlled by an animation
	if (m_isCameraSlaved) {
		// find object named m_cameraSlaveObjectName
		Object * obj = TheScriptEngine->getUnitNamed(m_cameraSlaveObjectName);
		
		if (obj != NULL) {
			// dig out the drawable
			Drawable * draw = obj->getDrawable();
			if (draw != NULL) {

				// dig out the first draw module with an ObjectDrawInterface
				for (DrawModule ** dm = draw->getDrawModules(); *dm; ++dm) {
					const ObjectDrawInterface* di = (*dm)->getObjectDrawInterface();
					if (di) {
						Matrix3D tm;
						di->clientOnly_getRenderObjBoneTransform(m_cameraSlaveObjectBoneName,&tm);

						// Ok, slam it into the camera!
						*transform = tm;

						//--------------------------------------------------------------------
						// WST 10.22.2002. Update the Listener positions used by audio system
						//--------------------------------------------------------------------
						Vector3 position = transform->Get_Translation();
						m_pos.x = position.X; 
						m_pos.y = position.Y; 
						m_pos.z = position.Z; 
						

						//DEBUG_LOG(("mpos x%f, y%f, z%f\n", m_pos.x, m_pos.y, m_pos.z ));

						break;
					}
				}

			} else {
				m_isCameraSlaved = false;
			}
		} else {
			m_isCameraSlaved = false;
		}
	}
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void W3DView::calcCameraConstraints()
{
//	const Matrix3D& cameraTransform = m_3DCamera->Get_Transform();

//	DEBUG_LOG(("*** rebuilding cam constraints\n"));

	// ok, now check to ensure that we can't see outside the map region,
	// and twiddle the camera if needed
	if (TheTerrainLogic)
	{
		Region3D mapRegion;
		TheTerrainLogic->getExtent( &mapRegion );
		
	/*
		Note the following restrictions on camera constraints!

		-- they assume that all maps are height 'm_groundLevel' at the edges.
				(since you need to add some "buffer" around the edges of your map
				anyway, this shouldn't be an issue.)

		-- for angles/pitches other than zero, it may show boundaries.
				since we currently plan the game to be restricted to this,
				it shouldn't be an issue.

	*/
		Real maxEdgeZ = m_groundLevel;
//		const Real BORDER_FUDGE = MAP_XY_FACTOR * 1.414f;
		Coord3D center, bottom;
		ICoord2D screen;

		//Pick at the center
		screen.x=0.5f*getWidth()+m_originX;
		screen.y=0.5f*getHeight()+m_originY;

		Vector3 rayStart,rayEnd;

		getPickRay(&screen,&rayStart,&rayEnd);

		center.x = Vector3::Find_X_At_Z(maxEdgeZ, rayStart, rayEnd);
		center.y = Vector3::Find_Y_At_Z(maxEdgeZ, rayStart, rayEnd);
		center.z = maxEdgeZ;

		screen.y = m_originY+ 0.95f*getHeight();
 		getPickRay(&screen,&rayStart,&rayEnd);
 		bottom.x = Vector3::Find_X_At_Z(maxEdgeZ, rayStart, rayEnd);
		bottom.y = Vector3::Find_Y_At_Z(maxEdgeZ, rayStart, rayEnd);
		bottom.z = maxEdgeZ;
		center.x -= bottom.x;
		center.y -= bottom.y;

		Real offset = center.length();

		if (TheGlobalData->m_debugAI) {
			offset = -1000; // push out the constraints so we can look at staging areas.
		}

		m_cameraConstraint.lo.x = mapRegion.lo.x + offset;
		m_cameraConstraint.hi.x = mapRegion.hi.x - offset;
		// this looks inverted, but is correct
		m_cameraConstraint.lo.y = mapRegion.lo.y + offset;
		m_cameraConstraint.hi.y = mapRegion.hi.y - offset;
		m_cameraConstraintValid = true;
	}
}

//-------------------------------------------------------------------------------------------------
/** Returns a world-space ray originating at a given screen pixel position
	and ending at the far clip plane for current camera.  Screen coordinates
	assumed in absolute values relative to full display resolution.*/
//-------------------------------------------------------------------------------------------------
void W3DView::getPickRay(const ICoord2D *screen, Vector3 *rayStart, Vector3 *rayEnd)
{
	Real logX,logY;

	//W3D Screen coordinates are -1 to 1, so we need to do some conversion:
	PixelScreenToW3DLogicalScreen(screen->x - m_originX,screen->y - m_originY, &logX, &logY,getWidth(),getHeight());

	*rayStart = m_3DCamera->Get_Position();	//get camera location
	m_3DCamera->Un_Project(*rayEnd,Vector2(logX,logY));	//get world space point
	*rayEnd -= *rayStart;	//vector camera to world space point
	rayEnd->Normalize();	//make unit vector
	*rayEnd *= m_3DCamera->Get_Depth();	//adjust length to reach far clip plane
	*rayEnd += *rayStart;	//get point on far clip plane along ray from camera.
}

//-------------------------------------------------------------------------------------------------
/** set the transform matrix of m_3DCamera, based on m_pos & m_angle */
//-------------------------------------------------------------------------------------------------
void W3DView::setCameraTransform( void )
{
	m_cameraHasMovedSinceRequest = true;
	Matrix3D cameraTransform( 1 );
	
	Real nearZ, farZ;
	// m_3DCamera->Get_Clip_Planes(nearZ, farZ);
	// Set the near to MAP_XY_FACTOR.  Improves zbuffer resolution.
	nearZ = MAP_XY_FACTOR; 
	farZ = 1200.0f;

	if (m_useRealZoomCam)	//WST 10.19.2002
	{
		if (m_FXPitch<0.95f)
		{
			farZ = farZ / m_FXPitch; //Extend far Z when we pitch up for RealZoomCam
		}
	}
	else
	{
		if ((TheGlobalData && TheGlobalData->m_drawEntireTerrain) || (m_FXPitch<0.95f || m_zoom>1.05))
		{	//need to extend far clip plane so entire terrain can be visible
			farZ *= MAP_XY_FACTOR;
		}
	}

	m_3DCamera->Set_Clip_Planes(nearZ, farZ);
#if defined(_DEBUG) || defined(_INTERNAL)
	if (TheGlobalData->m_useCameraConstraints)
#endif
	{
		if (!m_cameraConstraintValid)
		{
			buildCameraTransform(&cameraTransform);
			m_3DCamera->Set_Transform( cameraTransform );
			calcCameraConstraints();
		}
		DEBUG_ASSERTLOG(m_cameraConstraintValid,("*** cam constraints are not valid!!!\n"));

		if (m_cameraConstraintValid)
		{
			Coord3D pos = *getPosition();
			pos.x = maxf(m_cameraConstraint.lo.x, pos.x);
			pos.x = minf(m_cameraConstraint.hi.x, pos.x);
			pos.y = maxf(m_cameraConstraint.lo.y, pos.y);
			pos.y = minf(m_cameraConstraint.hi.y, pos.y);
			setPosition(&pos);
		}
	}

#if defined(_DEBUG) || defined(_INTERNAL)
	m_3DCamera->Set_View_Plane( m_FOV, -1 );
#endif

	// rebuild it (even if we just did it due to camera constraints)
	buildCameraTransform( &cameraTransform );
	m_3DCamera->Set_Transform( cameraTransform );

	if (TheTerrainRenderObject) 
	{
		RefRenderObjListIterator *it = W3DDisplay::m_3DScene->createLightsIterator();
		TheTerrainRenderObject->updateCenter(m_3DCamera, it);
		if (it) 
		{
		 W3DDisplay::m_3DScene->destroyLightsIterator(it);
		 it = NULL;
		}
	}
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void W3DView::init( void )
{
	// extend View functionality
	View::init();
	setName("W3DView");

	// Re-read camera offset from GlobalData here (not in constructor) because
	// W3DView is constructed before GameData.ini is fully parsed, leaving
	// m_cameraHeight/m_cameraPitch/m_cameraYaw at their 0.0f defaults.
	// By init() time all INI files have been loaded and the values are valid.
	// Fallback constants match retail GameData.ini defaults if INI still returns 0.
	{
		Real camHeight = TheGlobalData->m_cameraHeight;
		Real camPitch  = TheGlobalData->m_cameraPitch;
		Real camYaw    = TheGlobalData->m_cameraYaw;
		if (camHeight <= 0.0f) camHeight = 232.0f;   // retail GameData.ini default
		if (camPitch  <= 0.0f) camPitch  = 37.5f;    // retail GameData.ini default (degrees)
		// Publish the validated values; every other site that needs tan(pitch) reads
		// these rather than the raw globals.
		m_iniCamHeight = camHeight;
		m_iniCamPitch  = camPitch;
		m_iniCamYaw    = camYaw;
		m_cameraOffset.z = camHeight;
		m_cameraOffset.y = -(m_cameraOffset.z / tan(camPitch * (PI / 180.0f)));
		m_cameraOffset.x = -(m_cameraOffset.y * tan(camYaw   * (PI / 180.0f)));
		DEBUG_LOG(("BGFX CAM INIT: height=%.1f pitch=%.1f yaw=%.1f -> offZ=%.1f offY=%.1f offX=%.1f\n",
			camHeight, camPitch, camYaw,
			m_cameraOffset.z, m_cameraOffset.y, m_cameraOffset.x));
	}

	// set default camera "lookat" point
	Coord3D pos;
	pos.x = 87.0f;
	pos.y = 77.0f;
	pos.z = 0;

	pos.x *= MAP_XY_FACTOR;
	pos.y *= MAP_XY_FACTOR;

	setPosition(&pos);

	// create our 3D camera
	m_3DCamera = NEW_REF( CameraClass, () );


	setCameraTransform();

	// create our 2D camera for the GUI overlay
	m_2DCamera = NEW_REF( CameraClass, () );
	m_2DCamera->Set_Position( Vector3( 0, 0, 1 ) );
	Vector2 min = Vector2( -1, -0.75f );
	Vector2 max = Vector2( +1, +0.75f );
	m_2DCamera->Set_View_Plane( min, max );		
	m_2DCamera->Set_Clip_Planes( 0.995f, 2.0f );

	m_cameraConstraintValid = false;

	m_scrollAmountCutoff = TheGlobalData->m_scrollAmountCutoff;

}  // end init

//-------------------------------------------------------------------------------------------------
const Coord3D& W3DView::get3DCameraPosition() const
{
	Vector3 camera = m_3DCamera->Get_Position();
	static Coord3D pos;
	pos.set( camera.X, camera.Y, camera.Z );
	return pos;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void W3DView::reset( void )
{
	View::reset();

	// Just in case...
	setTimeMultiplier(1); // Set time rate back to 1.

	Coord3D arbitraryPos = { 0, 0, 0 };
	// Just move the camera to 0, 0, 0. It'll get repositioned at the beginning of the next game
	// anyways.
	resetCamera(&arbitraryPos, 1, 0.0f, 0.0f);

	setViewFilter(FT_VIEW_DEFAULT);

	Coord2D gb = { 0,0 };
	setGuardBandBias( &gb );
}

//-------------------------------------------------------------------------------------------------
/** draw worker for drawables in the view region */
//-------------------------------------------------------------------------------------------------
static void drawDrawable( Drawable *draw, void *userData )
{

	draw->draw( (View *)userData );

}  // end drawDrawable

// ------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
static void drawTerrainNormal( Drawable *draw, void *userData )
{
	UnsignedInt color = GameMakeColor( 255, 255, 0, 255 );
  if (TheTerrainLogic)
  {
    Coord3D pos = *draw->getPosition();
    Coord3D normal;
    pos.z = TheTerrainLogic->getGroundHeight(pos.x, pos.y, &normal);
    const Real NORMLEN = 20;
    normal.x = pos.x + normal.x * NORMLEN;
    normal.y = pos.y + normal.y * NORMLEN;
    normal.z = pos.z + normal.z * NORMLEN;
    ICoord2D start, end;
		TheTacticalView->worldToScreen(&pos, &start);
		TheTacticalView->worldToScreen(&normal, &end);
		TheDisplay->drawLine(start.x, start.y, end.x, end.y, 1.0f, color);
  }
}

#if defined(_DEBUG) || defined(_INTERNAL)
// ------------------------------------------------------------------------------------------------
// Draw a crude circle. Appears on top of any world geometry
// ------------------------------------------------------------------------------------------------
void drawDebugCircle( const Coord3D & center, Real radius, Real width, Color color )
{
  const Real inc = PI/4.0f;
  Real angle = 0.0f;
  Coord3D pnt, lastPnt;
  ICoord2D start, end;
  Bool endValid, startValid;

  lastPnt.x = center.x + radius * (Real)cos(angle);
  lastPnt.y = center.y + radius * (Real)sin(angle);
  lastPnt.z = center.z;
  endValid = ( TheTacticalView->worldToScreenTriReturn( &lastPnt, &end ) != View::WTS_INVALID );
  
  for( angle = inc; angle <= 2.0f * PI; angle += inc )
  {
    pnt.x = center.x + radius * (Real)cos(angle);
    pnt.y = center.y + radius * (Real)sin(angle);
    pnt.z = center.z;
    startValid = ( TheTacticalView->worldToScreenTriReturn( &pnt, &start ) != View::WTS_INVALID );
    
    if ( startValid && endValid ) 
      TheDisplay->drawLine( start.x, start.y, end.x, end.y, width, color );
    
    lastPnt = pnt;
    end = start;
    endValid = startValid;
  }
}

void drawDrawableExtents( Drawable *draw, void *userData );  // FORWARD DECLARATION
// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
static void drawContainedDrawable( Object *obj, void *userData )
{
	Drawable *draw = obj->getDrawable();

	if( draw )
		drawDrawableExtents( draw, userData );

}  // end drawContainedDrawable

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
static void drawDrawableExtents( Drawable *draw, void *userData )
{
	UnsignedInt color = GameMakeColor( 0, 255, 0, 255 );

	switch( draw->getDrawableGeometryInfo().getGeomType() )
	{

		//---------------------------------------------------------------------------------------------
		case GEOMETRY_BOX:
		{
			Real angle = draw->getOrientation();
			Real c = (Real)cos(angle);
			Real s = (Real)sin(angle);
			Real exc = draw->getDrawableGeometryInfo().getMajorRadius()*c;
			Real eyc = draw->getDrawableGeometryInfo().getMinorRadius()*c;
			Real exs = draw->getDrawableGeometryInfo().getMajorRadius()*s;
			Real eys = draw->getDrawableGeometryInfo().getMinorRadius()*s;
			Coord3D pts[4];
			pts[0].x = draw->getPosition()->x - exc - eys;
			pts[0].y = draw->getPosition()->y + eyc - exs;
			pts[0].z = 0;
			pts[1].x = draw->getPosition()->x + exc - eys;
			pts[1].y = draw->getPosition()->y + eyc + exs;
			pts[1].z = 0;
			pts[2].x = draw->getPosition()->x + exc + eys;
			pts[2].y = draw->getPosition()->y - eyc + exs;
			pts[2].z = 0;
			pts[3].x = draw->getPosition()->x - exc + eys;
			pts[3].y = draw->getPosition()->y - eyc - exs;
			pts[3].z = 0;
			Real z = draw->getPosition()->z;
			for( int i = 0; i < 2; i++ )
			{

				for (int corner = 0; corner < 4; corner++)
				{
					ICoord2D start, end;
					pts[corner].z = z;
					pts[(corner+1)&3].z = z;
					TheTacticalView->worldToScreen(&pts[corner], &start);
					TheTacticalView->worldToScreen(&pts[(corner+1)&3], &end);
					TheDisplay->drawLine(start.x, start.y, end.x, end.y, 1.0f, color);
				}

				z += draw->getDrawableGeometryInfo().getMaxHeightAbovePosition();

			}  // end for i

			break;

		}  // end case box

		//---------------------------------------------------------------------------------------------
		case GEOMETRY_SPHERE:	// not quite right, but close enough
		case GEOMETRY_CYLINDER:
		{ 
      Coord3D center = *draw->getPosition();
      const Real radius = draw->getDrawableGeometryInfo().getMajorRadius();

			// draw cylinder
			for( int i=0; i<2; i++ )
			{
        drawDebugCircle( center, radius, 1.0f, color );

        // next time 'round, draw the top of the cylinder
        center.z += draw->getDrawableGeometryInfo().getMaxHeightAbovePosition();
			}	// end for i

			// draw centerline
      ICoord2D start, end;
      center = *draw->getPosition();
      TheTacticalView->worldToScreen( &center, &start );
      center.z += draw->getDrawableGeometryInfo().getMaxHeightAbovePosition();
      TheTacticalView->worldToScreen( &center, &end );
			TheDisplay->drawLine( start.x, start.y, end.x, end.y, 1.0f, color );

			break;

		}	// case CYLINDER

	} // end switch

	// draw any extents for things that are contained by this
	Object *obj = draw->getObject();
	if( obj )
	{
		ContainModuleInterface *contain = obj->getContain();

		if( contain )
			contain->iterateContained( drawContainedDrawable, userData, FALSE );

	}  // end if

}  // end drawDrawableExtents


void drawAudioLocations( Drawable *draw, void *userData );
// ------------------------------------------------------------------------------------------------
// Helper for drawAudioLocations
// ------------------------------------------------------------------------------------------------
static void drawContainedAudioLocations( Object *obj, void *userData )
{
  Drawable *draw = obj->getDrawable();
  
  if( draw )
    drawAudioLocations( draw, userData );
  
}  // end drawContainedAudio


//-------------------------------------------------------------------------------------------------
// Draw the location of audio objects in the world
//-------------------------------------------------------------------------------------------------
static void drawAudioLocations( Drawable *draw, void *userData )
{
  // draw audio for things that are contained by this
  Object *obj = draw->getObject();
  if( obj )
  {
    ContainModuleInterface *contain = obj->getContain();
    
    if( contain )
      contain->iterateContained( drawContainedAudioLocations, userData, FALSE );
    
  }  // end if

  const ThingTemplate * thingTemplate = draw->getTemplate();

  if ( thingTemplate == NULL || thingTemplate->getEditorSorting() != ES_AUDIO )
  {
    return; // All done
  }

  // Copied in hideously inappropriate code copying ways from DrawObject.cpp
  // Should definately be a global, probably read in from an INI file <gasp>
  static const Int poleHeight = 20;
  static const Int flagHeight = 10;
  static const Int flagWidth = 10;
  const Color color = GameMakeColor(0x25, 0x25, 0xEF, 0xFF);

  // Draw flag for audio-only objects:
  //  *
  //  * *
  //  *   *
  //  *     *
  //  *   *
  //  * *
  //  *
  //  *
  //  *
  //  *
  //  *

  Coord3D worldPoint;
  ICoord2D start, end;

  worldPoint = *draw->getPosition();
  TheTacticalView->worldToScreen( &worldPoint, &start );
  worldPoint.z += poleHeight;
  TheTacticalView->worldToScreen( &worldPoint, &end );
  TheDisplay->drawLine( start.x, start.y, end.x, end.y, 1.0f, color );
  
  worldPoint.z -= flagHeight / 2;
  worldPoint.x += flagWidth;
  TheTacticalView->worldToScreen( &worldPoint, &start );
  TheDisplay->drawLine( start.x, start.y, end.x, end.y, 1.0f, color );

  worldPoint.z -= flagHeight / 2;
  worldPoint.x -= flagWidth;
  TheTacticalView->worldToScreen( &worldPoint, &end );
  TheDisplay->drawLine( start.x, start.y, end.x, end.y, 1.0f, color );
}

//-------------------------------------------------------------------------------------------------
// Draw the radii of sounds attached to any type of object. 
//-------------------------------------------------------------------------------------------------
static void drawAudioRadii( const Drawable * drawable )
{
  
  // Draw radii, if sound is playing
  const AudioEventRTS * ambientSound = drawable->getAmbientSound();
  
  if ( ambientSound && ambientSound->isCurrentlyPlaying() )
  {
    const AudioEventInfo * ambientInfo = ambientSound->getAudioEventInfo();
    
    if ( ambientInfo == NULL )
    {
      // I don't think that's right...
      OutputDebugString( ("Playing sound has NULL AudioEventInfo?\n" ) );
      
      if ( TheAudio != NULL )
      {
        ambientInfo = TheAudio->findAudioEventInfo( ambientSound->getEventName() );
      }
    }
    
    if ( ambientInfo != NULL )
    {
      // Colors match those used in WorldBuilder
      drawDebugCircle( *drawable->getPosition(), ambientInfo->m_minDistance, 1.0f, GameMakeColor(0x00, 0x00, 0xFF, 0xFF) );
      drawDebugCircle( *drawable->getPosition(), ambientInfo->m_maxDistance, 1.0f, GameMakeColor(0xFF, 0x00, 0xFF, 0xFF) );
    }
  }
}

#endif

//-------------------------------------------------------------------------------------------------
/** An opportunity to draw something after all drawables have been drawn once */
//-------------------------------------------------------------------------------------------------
static void drawablePostDraw( Drawable *draw, void *userData )
{
	Real FXPitch = TheTacticalView->getFXPitch();
	if (draw->isDrawableEffectivelyHidden() || FXPitch < 0.0f)
		return;

	Object* obj = draw->getObject();
	Int localPlayerIndex = ThePlayerList ? ThePlayerList->getLocalPlayer()->getPlayerIndex() : 0;
#if defined(_DEBUG) || defined(_INTERNAL)
	ObjectShroudStatus ss = (!obj || !TheGlobalData->m_shroudOn) ? OBJECTSHROUD_CLEAR : obj->getShroudedStatus(localPlayerIndex);
#else
	ObjectShroudStatus ss = (!obj) ? OBJECTSHROUD_CLEAR : obj->getShroudedStatus(localPlayerIndex);
#endif
	if (ss > OBJECTSHROUD_PARTIAL_CLEAR)
		return;

	// draw the any "icon" UI for a drawable (health bars, veterency, etc);
	
	//*****
	//@TODO: Create a way to reject this call easily -- like objects that have no compatible modules.
	//*****
	//if( draw->getStatusBits() )
	//{
			draw->drawIconUI();
	//}

#if defined(_DEBUG) || defined(_INTERNAL)
	// debug collision extents
	if( TheGlobalData->m_showCollisionExtents )
	  drawDrawableExtents( draw, userData );

  if ( TheGlobalData->m_showAudioLocations )
    drawAudioLocations( draw, userData );
#endif

	// debug terrain normals at object positions
	if( TheGlobalData->m_showTerrainNormals )
	  drawTerrainNormal( draw, userData );

	TheGameClient->incrementRenderedObjectCount();

}  // end drawablePostDraw

//-------------------------------------------------------------------------------------------------
// Display AI debug visuals
//-------------------------------------------------------------------------------------------------
static void renderAIDebug( void )
{
}

// =============================================================================
// Stub implementations for virtual methods not yet ported to BGFX
// =============================================================================
void W3DView::setFieldOfView( Real angle )
{
	View::setFieldOfView( angle );

	// Retail only refreshed the transform in debug/internal builds, since
	// recalculating the camera every frame is wasteful.
#if defined(_DEBUG) || defined(_INTERNAL)
	setCameraTransform();
#endif
}

View::WorldToScreenReturn W3DView::worldToScreenTriReturn( const Coord3D *w, ICoord2D *s )
{
	if (s) { s->x = 0; s->y = 0; }
	return WTS_INSIDE_FRUSTUM;
}

void W3DView::screenToWorld( const ICoord2D *s, Coord3D *w )
{
	if (w) { w->x = 0; w->y = 0; w->z = 0; }
}

void W3DView::screenToTerrain( const ICoord2D *screen, Coord3D *world )
{
	if (world) { world->x = 0; world->y = 0; world->z = 0; }
}

void W3DView::screenToWorldAtZ( const ICoord2D *s, Coord3D *w, Real z )
{
	if (w) { w->x = 0; w->y = 0; w->z = z; }
}

void W3DView::setCameraLock( ObjectID id )
{
	// If we are disabling camera movements, do not lock onto the object.
	if (TheGlobalData->m_disableCameraMovement && id != INVALID_ID) {
		return;
	}
	View::setCameraLock(id);
	m_doingScriptedCameraLock = FALSE;
}
void W3DView::setSnapMode( CameraLockType lockType, Real lockDist )
{
	View::setSnapMode(lockType, lockDist);
	m_doingScriptedCameraLock = TRUE;
}
void W3DView::shake( const Coord3D *epicenter, CameraShakeType shakeType ) { /* TODO: BGFX port */ }

Bool W3DView::setViewFilterMode( enum FilterModes filterMode )
{
	m_viewFilterMode = filterMode;
	return TRUE;
}

Bool W3DView::setViewFilter( enum FilterTypes filter )
{
	m_viewFilter = filter;
	return TRUE;
}

void W3DView::setViewFilterPos( const Coord3D *pos ) { /* TODO: BGFX port */ }

void W3DView::setFadeParameters( Int fadeFrames, Int direction ) { /* TODO: BGFX port */ }

void W3DView::set3DWireFrameMode( Bool enable )
{
	m_nextWireFrameEnabled = enable;
}

Bool W3DView::updateCameraMovements()
{
	Bool didUpdate = false;
	if (m_doingZoomCamera)     { zoomCameraOneFrame();  didUpdate = true; }
	if (m_doingPitchCamera)    { pitchCameraOneFrame(); didUpdate = true; }
	if (m_doingRotateCamera)   { m_previousLookAtPosition = *getPosition(); rotateCameraOneFrame(); didUpdate = true; }
	else if (m_doingMoveCameraOnWaypointPath) { m_previousLookAtPosition = *getPosition(); moveAlongWaypointPath(TheW3DFrameLengthInMsec); didUpdate = true; }
	if (m_doingScriptedCameraLock) { didUpdate = true; }
	return didUpdate;
}

void W3DView::drawView( void ) { DRAW(); }

void W3DView::updateView( void ) { UPDATE(); }

void W3DView::draw( void )
{
	Bool skipRender = false;
	Bool doExtraRender = false;
	CustomScenePassModes customScenePassMode = SCENE_PASS_DEFAULT;
	Bool preRenderResult = false;

	if (m_viewFilterMode &&
		m_viewFilter > FT_NULL_FILTER &&
		m_viewFilter < FT_MAX)
	{
		preRenderResult = W3DShaderManager::filterPreRender(m_viewFilter, skipRender, customScenePassMode);
		if (!skipRender && getCameraLock())
		{
			Object* cameraLockObj = TheGameLogic->findObjectByID(getCameraLock());
			if (cameraLockObj)
			{
				Drawable *drawable = cameraLockObj->getDrawable();
				drawable->setDrawableHidden(true);
			}
		}
	}

	if (!skipRender)
	{
		// Render 3D scene from our camera
		W3DDisplay::m_3DScene->setCustomPassMode(customScenePassMode);
		if (m_isWireFrameEnabled)
			W3DDisplay::m_3DScene->Set_Extra_Pass_Polygon_Mode(SceneClass::EXTRA_PASS_CLEAR_LINE);
		W3DDisplay::m_3DScene->doRender( m_3DCamera );
		W3DDisplay::m_3DScene->Set_Extra_Pass_Polygon_Mode(SceneClass::EXTRA_PASS_DISABLE);
		m_isWireFrameEnabled = m_nextWireFrameEnabled;
	}

	if (m_viewFilterMode &&
		m_viewFilter > FT_NULL_FILTER &&
		m_viewFilter < FT_MAX)
	{
		Coord2D deltaScroll;
		calcDeltaScroll(deltaScroll);
		Bool continueTheEffect = false;
		if (preRenderResult)
			continueTheEffect = W3DShaderManager::filterPostRender(m_viewFilter, m_viewFilterMode, deltaScroll, doExtraRender);
		if (!skipRender && getCameraLock())
		{
			Object* cameraLockObj = TheGameLogic->findObjectByID(getCameraLock());
			if (cameraLockObj)
			{
				Drawable *drawable = cameraLockObj->getDrawable();
				drawable->setDrawableHidden(false);
				RenderInfoClass rinfo(*m_3DCamera);
				m_3DCamera->Apply();
				TheDX8MeshRenderer.Set_Camera(&rinfo.Camera);
				W3DDisplay::m_3DScene->renderSpecificDrawables(rinfo, 1, &drawable);
				WW3D::Flush(rinfo);
			}
		}
		if (!continueTheEffect)
		{
			m_viewFilter = FT_VIEW_DEFAULT;
			m_viewFilterMode = FM_VIEW_DEFAULT;
		}
	}

	if (doExtraRender)
	{
		DX8Wrapper::Clear(false, true, Vector3(0.0f, 0.0f, 0.0f), TheWaterTransparency->m_minWaterOpacity);
		W3DDisplay::m_3DScene->setCustomPassMode(SCENE_PASS_DEFAULT);
		W3DDisplay::m_3DScene->doRender( m_3DCamera );
		Coord2D deltaScroll;
		W3DShaderManager::filterPostRender(m_viewFilter, m_viewFilterMode, deltaScroll, doExtraRender);
	}

	Region3D axisAlignedRegion;
	getAxisAlignedViewRegion(axisAlignedRegion);

	TheGameClient->resetRenderedObjectCount();
	TheGameClient->iterateDrawablesInRegion( &axisAlignedRegion, drawablePostDraw, this );
	TheGameClient->flushTextBearingDrawables();

	// Render 2D scene
	W3DDisplay::m_2DScene->doRender( m_2DCamera );
}

void W3DView::update( void )
{
	Bool recalcCamera = false;
	Bool didScriptedMovement = false;

	if (TheTerrainRenderObject && TheTerrainRenderObject->doesNeedFullUpdate()) {
		RefRenderObjListIterator *it = W3DDisplay::m_3DScene->createLightsIterator();
		TheTerrainRenderObject->updateCenter(m_3DCamera, it);
		if (it) {
			W3DDisplay::m_3DScene->destroyLightsIterator(it);
			it = NULL;
		}
	}

	static Real followFactor = -1;
	ObjectID cameraLock = getCameraLock();
	if (cameraLock == INVALID_ID) { followFactor = -1; }
	if (cameraLock != INVALID_ID)
	{
		m_doingMoveCameraOnWaypointPath = false;
		m_CameraArrivedAtWaypointOnPathFlag = false;
		Object* cameraLockObj = TheGameLogic->findObjectByID(cameraLock);
		Bool loseLock = false;
		if (cameraLockObj == NULL) { loseLock = true; }
		if (loseLock)
		{
			setCameraLock(INVALID_ID);
			setCameraLockDrawable(NULL);
			followFactor = -1;
		}
		else
		{
			if (followFactor < 0) { followFactor = 0.05f; }
			else { followFactor += 0.05f; if (followFactor > 1.0f) followFactor = 1.0f; }
			if (getCameraLockDrawable() != NULL)
			{
				Drawable* cameraLockDrawable = (Drawable *)getCameraLockDrawable();
				if (!cameraLockDrawable) { setCameraLockDrawable(NULL); }
				else
				{
					Coord3D pos;
					Real boundingSphereRadius;
					Matrix3D transform;
					if (cameraLockDrawable->clientOnly_getFirstRenderObjInfo(&pos, &boundingSphereRadius, &transform))
					{
						Vector3 zaxis(0,0,1);
						Vector3 objPos(pos.x, pos.y, pos.z);
						objPos += boundingSphereRadius * 1.0f * zaxis;
						Vector3 objview = transform.Get_X_Vector();
						Vector3 camtran = objPos - objview * boundingSphereRadius*4.5f;
						Vector3 prevCamTran = m_3DCamera->Get_Position();
						Vector3 tranDiff = (camtran - prevCamTran);
						camtran = prevCamTran + tranDiff * 0.1f;
						Matrix3D camXForm;
						camXForm.Look_At(camtran, objPos, 0);
						m_3DCamera->Set_Transform(camXForm);
						recalcCamera = false;
					}
				}
			}
			else
			{
				Coord3D objpos = *cameraLockObj->getPosition();
				Coord3D curpos = *getPosition();
				Real snapThreshSqr = sqr(TheGlobalData->m_partitionCellSize);
				Real curDistSqr = sqr(curpos.x - objpos.x) + sqr(curpos.y - objpos.y);
				if (m_snapImmediate) { curpos.x = objpos.x; curpos.y = objpos.y; }
				else
				{
					Real dx = objpos.x - curpos.x;
					Real dy = objpos.y - curpos.y;
					if (m_lockType == LOCK_TETHER)
					{
						if (curDistSqr >= snapThreshSqr)
						{
							Real ratio = 1.0f - snapThreshSqr / curDistSqr;
							curpos.x += dx * ratio * 0.5f;
							curpos.y += dy * ratio * 0.5f;
						}
						else
						{
							Real ratio = 0.01f * m_lockDist;
							curpos.x += dx * ratio;
							curpos.y += dy * ratio;
						}
					}
					else
					{
						curpos.x += dx * followFactor;
						curpos.y += dy * followFactor;
					}
				}
				if (!(TheScriptEngine->isTimeFrozenDebug() || TheScriptEngine->isTimeFrozenScript()) && !TheGameLogic->isGamePaused())
					m_previousLookAtPosition = *getPosition();
				setPosition(&curpos);
				if (m_lockType == LOCK_FOLLOW)
				{
					if (cameraLockObj->isUsingAirborneLocomotor() && cameraLockObj->isAboveTerrainOrWater())
					{
						Real oldZRot = m_angle;
						Real idealZRot = cameraLockObj->getOrientation() - M_PI_2;
						normAngle(oldZRot);
						normAngle(idealZRot);
						Real diff = idealZRot - oldZRot;
						normAngle(diff);
						if (m_snapImmediate) { m_angle = idealZRot; }
						else { m_angle += diff * 0.1f; }
						normAngle(m_angle);
					}
				}
				if (m_snapImmediate) m_snapImmediate = FALSE;
				m_groundLevel = objpos.z;
				didScriptedMovement = true;
				recalcCamera = true;
			}
		}
	}

	if (!(TheScriptEngine->isTimeFrozenDebug()) && !TheGameLogic->isGamePaused()) {
		if (updateCameraMovements()) { didScriptedMovement = true; recalcCamera = true; }
	} else {
		if (m_doingRotateCamera || m_doingMoveCameraOnWaypointPath || m_doingPitchCamera || m_doingZoomCamera || m_doingScriptedCameraLock)
			didScriptedMovement = true;
	}

	// Process camera shake
	if (m_shakeIntensity > 0.01f)
	{
		m_shakeOffset.x = m_shakeIntensity * m_shakeAngleCos;
		m_shakeOffset.y = m_shakeIntensity * m_shakeAngleSin;
		const Real dampingCoeff = 0.75f;
		m_shakeIntensity *= dampingCoeff;
		m_shakeAngleCos = -m_shakeAngleCos;
		m_shakeAngleSin = -m_shakeAngleSin;
		recalcCamera = true;
	}
	else
	{
		m_shakeIntensity = 0.0f;
		m_shakeOffset.x = 0.0f;
		m_shakeOffset.y = 0.0f;
	}

	if (CameraShakerSystem.IsCameraShaking()) { recalcCamera = true; }

	m_terrainHeightUnderCamera = getHeightAroundPos(m_pos.x, m_pos.y);
	m_currentHeightAboveGround = m_cameraOffset.z * m_zoom - m_terrainHeightUnderCamera;
	if (TheTerrainLogic && TheGlobalData && TheInGameUI && m_okToAdjustHeight && !TheGameLogic->isGamePaused())
	{
		Real desiredHeight = (m_terrainHeightUnderCamera + m_heightAboveGround);
		Real desiredZoom = desiredHeight / m_cameraOffset.z;
		if (didScriptedMovement || (TheGameLogic->isInReplayGame() && TheGlobalData->m_useCameraInReplay))
			m_heightAboveGround = m_currentHeightAboveGround;
		if (TheInGameUI->isScrolling())
		{
			if (m_scrollAmount.length() < m_scrollAmountCutoff || (m_currentHeightAboveGround < m_minHeightAboveGround) || (TheGlobalData->m_enforceMaxCameraHeight && m_currentHeightAboveGround > m_maxHeightAboveGround))
			{
				Real zoomAdj = (desiredZoom - m_zoom) * TheGlobalData->m_cameraAdjustSpeed;
				if (fabs(zoomAdj) >= 0.0001) { m_zoom += zoomAdj; recalcCamera = true; }
			}
		}
		else
		{
			Real zoomAdj = (m_zoom - desiredZoom) * TheGlobalData->m_cameraAdjustSpeed;
			if (fabs(zoomAdj) >= 0.0001 && !didScriptedMovement) { m_zoom -= zoomAdj; recalcCamera = true; }
		}
	}
	if (TheScriptEngine->isTimeFast()) { return; }

	if ((recalcCamera) || (m_isCameraSlaved)) { setCameraTransform(); }

	Region3D axisAlignedRegion;
	getAxisAlignedViewRegion(axisAlignedRegion);

	if (WW3D::Get_Frame_Time())
		TheGameClient->iterateDrawablesInRegion( &axisAlignedRegion, drawDrawable, this );
}

void W3DView::getAxisAlignedViewRegion(Region3D &axisAlignedRegion)
{
	Coord3D box[4];
	getScreenCornerWorldPointsAtZ(&box[0], &box[1], &box[2], &box[3], 0.0f);

	axisAlignedRegion.lo = box[0];
	axisAlignedRegion.hi = box[0];
	for (Int i = 0; i < 4; i++)
	{
		if (box[i].x < axisAlignedRegion.lo.x) axisAlignedRegion.lo.x = box[i].x;
		if (box[i].y < axisAlignedRegion.lo.y) axisAlignedRegion.lo.y = box[i].y;
		if (box[i].x > axisAlignedRegion.hi.x) axisAlignedRegion.hi.x = box[i].x;
		if (box[i].y > axisAlignedRegion.hi.y) axisAlignedRegion.hi.y = box[i].y;
	}

	Region3D mapExtent;
	Real safeValue = 999999;
	TheTerrainLogic->getExtent(&mapExtent);
	axisAlignedRegion.lo.z = mapExtent.lo.z - safeValue;
	axisAlignedRegion.hi.z = mapExtent.hi.z + safeValue;

	axisAlignedRegion.lo.x -= (DRAWABLE_OVERSCAN + m_guardBandBias.x);
	axisAlignedRegion.lo.y -= (DRAWABLE_OVERSCAN + m_guardBandBias.y + 60.0f);
	axisAlignedRegion.hi.x += (DRAWABLE_OVERSCAN + m_guardBandBias.x);
	axisAlignedRegion.hi.y += (DRAWABLE_OVERSCAN + m_guardBandBias.y);
}

void W3DView::calcDeltaScroll(Coord2D &screenDelta)
{
	screenDelta.x = 0;
	screenDelta.y = 0;
	Vector3 prevPos(m_previousLookAtPosition.x, m_previousLookAtPosition.y, m_groundLevel);
	Vector3 prevScreen;
	if (m_3DCamera->Project(prevScreen, prevPos) != CameraClass::INSIDE_FRUSTUM) return;
	Vector3 pos(m_pos.x, m_pos.y, m_groundLevel);
	Vector3 screen;
	if (m_3DCamera->Project(screen, pos) != CameraClass::INSIDE_FRUSTUM) return;
	screenDelta.x = screen.X - prevScreen.X;
	screenDelta.y = screen.Y - prevScreen.Y;
}

void W3DView::rotateCameraOneFrame(void)
{
	m_rcInfo.curFrame++;
	if (TheGlobalData->m_disableCameraMovement) {
		if (m_rcInfo.curFrame >= m_rcInfo.numFrames + m_rcInfo.numHoldFrames) {
			m_doingRotateCamera = false;
			m_freezeTimeForCameraMovement = false;
		}
		return;
	}
	if (m_rcInfo.trackObject)
	{
		if (m_rcInfo.curFrame <= m_rcInfo.numFrames + m_rcInfo.numHoldFrames)
		{
			const Object *obj = TheGameLogic->findObjectByID(m_rcInfo.target.targetObjectID);
			if (obj) m_rcInfo.target.targetObjectPos = *obj->getPosition();
			const Vector2 dir(m_rcInfo.target.targetObjectPos.x - m_pos.x, m_rcInfo.target.targetObjectPos.y - m_pos.y);
			const Real dirLength = dir.Length();
			if (dirLength >= 0.1f)
			{
				Real angle = WWMath::Acos(dir.X / dirLength);
				if (dir.Y < 0.0f) angle = -angle;
				angle -= PI / 2;
				normAngle(angle);
				if (m_rcInfo.curFrame <= m_rcInfo.numFrames)
				{
					Real factor = m_rcInfo.ease(((Real)m_rcInfo.curFrame) / m_rcInfo.numFrames);
					Real angleDiff = angle - m_angle;
					normAngle(angleDiff);
					angleDiff *= factor;
					m_angle += angleDiff;
					normAngle(m_angle);
					m_timeMultiplier = m_rcInfo.startTimeMultiplier + REAL_TO_INT_FLOOR(0.5 + (m_rcInfo.endTimeMultiplier - m_rcInfo.startTimeMultiplier) * factor);
				}
				else { m_angle = angle; }
			}
		}
	}
	else if (m_rcInfo.curFrame <= m_rcInfo.numFrames)
	{
		Real factor = m_rcInfo.ease(((Real)m_rcInfo.curFrame) / m_rcInfo.numFrames);
		m_angle = WWMath::Lerp(m_rcInfo.angle.startAngle, m_rcInfo.angle.endAngle, factor);
		normAngle(m_angle);
		m_timeMultiplier = m_rcInfo.startTimeMultiplier + REAL_TO_INT_FLOOR(0.5 + (m_rcInfo.endTimeMultiplier - m_rcInfo.startTimeMultiplier) * factor);
	}
	if (m_rcInfo.curFrame >= m_rcInfo.numFrames + m_rcInfo.numHoldFrames) {
		m_doingRotateCamera = false;
		m_freezeTimeForCameraMovement = false;
		if (!m_rcInfo.trackObject) m_angle = m_rcInfo.angle.endAngle;
	}
}

void W3DView::zoomCameraOneFrame(void)
{
	m_zcInfo.curFrame++;
	if (TheGlobalData->m_disableCameraMovement) {
		if (m_zcInfo.curFrame >= m_zcInfo.numFrames) m_doingZoomCamera = false;
		return;
	}
	if (m_zcInfo.curFrame <= m_zcInfo.numFrames)
	{
		Real factor = m_zcInfo.ease(((Real)m_zcInfo.curFrame) / m_zcInfo.numFrames);
		m_zoom = WWMath::Lerp(m_zcInfo.startZoom, m_zcInfo.endZoom, factor);
	}
	if (m_zcInfo.curFrame >= m_zcInfo.numFrames) {
		m_doingZoomCamera = false;
		m_zoom = m_zcInfo.endZoom;
	}
}

void W3DView::pitchCameraOneFrame(void)
{
	m_pcInfo.curFrame++;
	if (TheGlobalData->m_disableCameraMovement) {
		if (m_pcInfo.curFrame >= m_pcInfo.numFrames) m_doingPitchCamera = false;
		return;
	}
	if (m_pcInfo.curFrame <= m_pcInfo.numFrames)
	{
		Real factor = m_pcInfo.ease(((Real)m_pcInfo.curFrame) / m_pcInfo.numFrames);
		m_FXPitch = WWMath::Lerp(m_pcInfo.startPitch, m_pcInfo.endPitch, factor);
	}
	if (m_pcInfo.curFrame >= m_pcInfo.numFrames) {
		m_doingPitchCamera = false;
		m_FXPitch = m_pcInfo.endPitch;
	}
}

void W3DView::moveAlongWaypointPath(Int milliseconds)
{
	m_mcwpInfo.elapsedTimeMilliseconds += milliseconds;
	if (TheGlobalData->m_disableCameraMovement) {
		if (m_mcwpInfo.elapsedTimeMilliseconds > m_mcwpInfo.totalTimeMilliseconds) {
			m_doingMoveCameraOnWaypointPath = false;
			m_freezeTimeForCameraMovement = false;
		}
		return;
	}
	if (m_mcwpInfo.elapsedTimeMilliseconds > m_mcwpInfo.totalTimeMilliseconds) {
		m_doingMoveCameraOnWaypointPath = false;
		m_CameraArrivedAtWaypointOnPathFlag = false;
		m_freezeTimeForCameraMovement = false;
		m_angle = m_mcwpInfo.cameraAngle[m_mcwpInfo.numWaypoints];
		m_groundLevel = m_mcwpInfo.groundHeight[m_mcwpInfo.numWaypoints];
		// Validated cached values, not the raw globals: tan(0) is a singularity.
		m_cameraOffset.y = -(m_cameraOffset.z / tan(m_iniCamPitch * (PI / 180.0)));
		m_cameraOffset.x = -(m_cameraOffset.y * tan(m_iniCamYaw * (PI / 180.0)));
		Coord3D pos = m_mcwpInfo.waypoints[m_mcwpInfo.numWaypoints];
		pos.z = 0;
		setPosition(&pos);
		m_cameraConstraint.lo.x = minf(m_cameraConstraint.lo.x, pos.x);
		m_cameraConstraint.hi.x = maxf(m_cameraConstraint.hi.x, pos.x);
		m_cameraConstraint.lo.y = minf(m_cameraConstraint.lo.y, pos.y);
		m_cameraConstraint.hi.y = maxf(m_cameraConstraint.hi.y, pos.y);
		return;
	}

	const Real totalTime = m_mcwpInfo.totalTimeMilliseconds;
	const Real deltaTime = m_mcwpInfo.ease(m_mcwpInfo.elapsedTimeMilliseconds / totalTime) -
		m_mcwpInfo.ease((m_mcwpInfo.elapsedTimeMilliseconds - milliseconds) / totalTime);
	m_mcwpInfo.curSegDistance += deltaTime * m_mcwpInfo.totalDistance;
	while (m_mcwpInfo.curSegDistance >= m_mcwpInfo.waySegLength[m_mcwpInfo.curSegment]) {
		if (m_doingMoveCameraOnWaypointPath) m_CameraArrivedAtWaypointOnPathFlag = true;
		m_mcwpInfo.curSegDistance -= m_mcwpInfo.waySegLength[m_mcwpInfo.curSegment];
		m_mcwpInfo.curSegment++;
		if (m_mcwpInfo.curSegment >= m_mcwpInfo.numWaypoints) {
			m_mcwpInfo.totalTimeMilliseconds = 0;
			return;
		}
	}
	Real avgFactor = 1.0 / m_mcwpInfo.rollingAverageFrames;
	m_mcwpInfo.curShutter--;
	if (m_mcwpInfo.curShutter > 0) return;
	m_mcwpInfo.curShutter = m_mcwpInfo.shutter;
	Real factor = m_mcwpInfo.curSegDistance / m_mcwpInfo.waySegLength[m_mcwpInfo.curSegment];
	if (m_mcwpInfo.curSegment == m_mcwpInfo.numWaypoints - 1)
		avgFactor = avgFactor + (1.0 - avgFactor) * factor;
	Real factor1 = 1.0 - factor;
	Real factor2 = 1.0 - factor1;
	Real angle1 = m_mcwpInfo.cameraAngle[m_mcwpInfo.curSegment];
	Real angle2 = m_mcwpInfo.cameraAngle[m_mcwpInfo.curSegment + 1];
	if (angle2 - angle1 > PI) angle1 += 2 * PI;
	if (angle2 - angle1 < -PI) angle1 -= 2 * PI;
	Real angle = angle1 * factor1 + angle2 * factor2;
	normAngle(angle);
	Real deltaAngle = angle - m_angle;
	normAngle(deltaAngle);
	m_angle += avgFactor * deltaAngle;
	normAngle(m_angle);
	Real timeMultiplier = m_mcwpInfo.timeMultiplier[m_mcwpInfo.curSegment] * factor1 +
		m_mcwpInfo.timeMultiplier[m_mcwpInfo.curSegment + 1] * factor2;
	m_timeMultiplier = REAL_TO_INT_FLOOR(0.5 + timeMultiplier);
	m_groundLevel = m_mcwpInfo.groundHeight[m_mcwpInfo.curSegment] * factor1 +
		m_mcwpInfo.groundHeight[m_mcwpInfo.curSegment + 1] * factor2;
	// Validated cached values, not the raw globals: tan(0) is a singularity.
	m_cameraOffset.y = -(m_cameraOffset.z / tan(m_iniCamPitch * (PI / 180.0)));
	m_cameraOffset.x = -(m_cameraOffset.y * tan(m_iniCamYaw * (PI / 180.0)));

	Coord3D start, mid, end;
	if (factor < 0.5) {
		start = m_mcwpInfo.waypoints[m_mcwpInfo.curSegment - 1];
		start.x += m_mcwpInfo.waypoints[m_mcwpInfo.curSegment].x;
		start.y += m_mcwpInfo.waypoints[m_mcwpInfo.curSegment].y;
		start.x /= 2; start.y /= 2;
		mid = m_mcwpInfo.waypoints[m_mcwpInfo.curSegment];
		end = m_mcwpInfo.waypoints[m_mcwpInfo.curSegment];
		end.x += m_mcwpInfo.waypoints[m_mcwpInfo.curSegment + 1].x;
		end.y += m_mcwpInfo.waypoints[m_mcwpInfo.curSegment + 1].y;
		end.x /= 2; end.y /= 2;
		factor += 0.5;
	} else {
		start = m_mcwpInfo.waypoints[m_mcwpInfo.curSegment];
		start.x += m_mcwpInfo.waypoints[m_mcwpInfo.curSegment + 1].x;
		start.y += m_mcwpInfo.waypoints[m_mcwpInfo.curSegment + 1].y;
		start.x /= 2; start.y /= 2;
		mid = m_mcwpInfo.waypoints[m_mcwpInfo.curSegment + 1];
		end = m_mcwpInfo.waypoints[m_mcwpInfo.curSegment + 1];
		end.x += m_mcwpInfo.waypoints[m_mcwpInfo.curSegment + 2].x;
		end.y += m_mcwpInfo.waypoints[m_mcwpInfo.curSegment + 2].y;
		end.x /= 2; end.y /= 2;
		factor -= 0.5;
	}
	Coord3D result = start;
	result.x += factor * (end.x - start.x);
	result.y += factor * (end.y - start.y);
	result.x += (1 - factor) * factor * (mid.x - end.x + mid.x - start.x);
	result.y += (1 - factor) * factor * (mid.y - end.y + mid.y - start.y);
	result.z = 0;
	static int s_camPosLogFrame = 0;
	if ((++s_camPosLogFrame % 300) == 0) {
		static FILE* s_camLog = NULL;
		if (!s_camLog) s_camLog = fopen("C:\\TheLab\\Development\\Generals-Modern\\bgfx_camera.log", "w");
		if (s_camLog) {
			fprintf(s_camLog, "CAMPOS frame=%d seg=%d/%d elapsed=%d/%d pos=(%.1f,%.1f)\n",
				s_camPosLogFrame, m_mcwpInfo.curSegment, m_mcwpInfo.numWaypoints,
				m_mcwpInfo.elapsedTimeMilliseconds, m_mcwpInfo.totalTimeMilliseconds,
				result.x, result.y);
			fflush(s_camLog);
		}
	}
	setPosition(&result);
	m_cameraConstraint.lo.x = minf(m_cameraConstraint.lo.x, result.x);
	m_cameraConstraint.hi.x = maxf(m_cameraConstraint.hi.x, result.x);
	m_cameraConstraint.lo.y = minf(m_cameraConstraint.lo.y, result.y);
	m_cameraConstraint.hi.y = maxf(m_cameraConstraint.hi.y, result.y);
}

Drawable *W3DView::pickDrawable( const ICoord2D *screen, Bool forceAttack, PickType pickType )
{
	return NULL;
}

Int W3DView::iterateDrawablesInRegion( IRegion2D *screenRegion,
	Bool (*callback)( Drawable *draw, void *userData ),
	void *userData )
{
	return 0;
}

void W3DView::scrollBy( Coord2D *delta )
{
	// if we have not moved, ignore
	if( delta && (delta->x != 0 || delta->y != 0) )
	{
		const Real SCROLL_RESOLUTION = 250.0f;

		Vector3 world, worldStart, worldEnd;
		Vector2 screen, start, end;

		m_scrollAmount = *delta;

		screen.X = delta->x;
		screen.Y = delta->y;

		start.X = getWidth();
		start.Y = getHeight();
		Real aspect = getWidth()/getHeight();
		end.X = start.X + delta->x * SCROLL_RESOLUTION;
		end.Y = start.Y + delta->y * SCROLL_RESOLUTION*aspect;

		if (m_3DCamera == NULL) return;
		m_3DCamera->Device_To_World_Space( start, &worldStart );
		m_3DCamera->Device_To_World_Space( end, &worldEnd );

		world.X = worldEnd.X - worldStart.X;
		world.Y = worldEnd.Y - worldStart.Y;
		world.Z = worldEnd.Z - worldStart.Z;

		// scroll by delta
		Coord3D pos = *getPosition();
		pos.x += world.X;
		pos.y += world.Y;
		// no change to z
		setPosition(&pos);

		m_doingRotateCamera = false;
		// set new camera position
		setCameraTransform();
	}  // end if
}  // end scrollBy
void W3DView::forceRedraw( void )
{
	// set the camera
	setCameraTransform();
}

void W3DView::setAngle( Real angle )
{
	// Normalize to +-PI.
	normAngle(angle);
	// call our base class, we are adding functionality
	View::setAngle( angle );

	m_doingMoveCameraOnWaypointPath = false;
	m_CameraArrivedAtWaypointOnPathFlag = false;
	m_doingRotateCamera = false;
	m_doingPitchCamera = false;
	m_doingZoomCamera = false;
	m_doingScriptedCameraLock = false;
	// set the camera
	setCameraTransform();
}
void W3DView::setPitch( Real angle )
{
	// call our base class, we are extending functionality
	View::setPitch( angle );

	m_doingMoveCameraOnWaypointPath = false;
	m_doingRotateCamera = false;
	m_doingPitchCamera = false;
	m_doingZoomCamera = false;
	m_doingScriptedCameraLock = false;
	// set the camera
	setCameraTransform();
}
void W3DView::setAngleAndPitchToDefault( void )
{
	// call our base class, we are adding functionality
	View::setAngleAndPitchToDefault();

	this->m_FXPitch = 1.0;

	// set the camera
	setCameraTransform();
}

void W3DView::lookAt( const Coord3D *o )
{
	if (o) { m_pos.x = o->x; m_pos.y = o->y; m_pos.z = o->z; }
}

void W3DView::initHeightForMap( void )
{
	m_groundLevel = TheTerrainLogic->getGroundHeight(m_pos.x, m_pos.y);
	const Real MAX_GROUND_LEVEL = 120.0; // jba - starting ground level cannot exceed this height.
	if (m_groundLevel > MAX_GROUND_LEVEL) {
		m_groundLevel = MAX_GROUND_LEVEL;
	}

	// Rebase the camera offset onto this map ground level. Without this the offset
	// keeps its startup value while m_groundLevel tracks real terrain, and the two
	// diverge until the height factor in buildCameraTransform goes negative.
	m_cameraOffset.z = m_groundLevel + m_iniCamHeight;
	m_cameraOffset.y = -(m_cameraOffset.z / tan(m_iniCamPitch * (PI / 180.0)));
	m_cameraOffset.x = -(m_cameraOffset.y * tan(m_iniCamYaw * (PI / 180.0)));
	m_cameraConstraintValid = false;	// possible ground level change invalidates cam constraints
	setCameraTransform();
}

void W3DView::moveCameraTo( const Coord3D *o, Int milliseconds, Int shutter, Bool orient, Real easeIn, Real easeOut )
{
	m_mcwpInfo.waypoints[0] = *getPosition();
	m_mcwpInfo.cameraAngle[0] = getAngle();
	m_mcwpInfo.waySegLength[0] = 0;

	m_mcwpInfo.waypoints[1] = *getPosition();
	m_mcwpInfo.waySegLength[1] = 0;

	m_mcwpInfo.waypoints[2] = *o;
	m_mcwpInfo.waySegLength[2] = 0;

	m_mcwpInfo.numWaypoints = 2;
	if (milliseconds < 1) milliseconds = 1;
	m_mcwpInfo.totalTimeMilliseconds = milliseconds;
	m_mcwpInfo.shutter = 1;
	m_mcwpInfo.ease.setEaseTimes(easeIn/milliseconds, easeOut/milliseconds);
	m_mcwpInfo.curSegment = 1;
	m_mcwpInfo.curSegDistance = 0;
	m_mcwpInfo.totalDistance = 0;

	setupWaypointPath(orient);
	if (m_mcwpInfo.totalTimeMilliseconds == 1) {
		// do it instantly.
		moveAlongWaypointPath(1);
		m_doingMoveCameraOnWaypointPath = true;
		m_CameraArrivedAtWaypointOnPathFlag = false;
	}
}

void W3DView::moveCameraAlongWaypointPath( Waypoint *pWay, Int milliseconds, Int shutter, Bool orient, Real easeIn, Real easeOut )
{
	const Real MIN_DELTA = MAP_XY_FACTOR;

	m_mcwpInfo.waypoints[0] = *getPosition();
	m_mcwpInfo.cameraAngle[0] = getAngle();
	m_mcwpInfo.waySegLength[0] = 0;
	m_mcwpInfo.waypoints[1] = *getPosition();
	m_mcwpInfo.numWaypoints = 1;
	if (milliseconds < 1) milliseconds = 1;
	m_mcwpInfo.totalTimeMilliseconds = milliseconds;
	m_mcwpInfo.shutter = shutter / TheW3DFrameLengthInMsec;
	if (m_mcwpInfo.shutter < 1) m_mcwpInfo.shutter = 1;
	m_mcwpInfo.ease.setEaseTimes(easeIn / milliseconds, easeOut / milliseconds);

	while (pWay && m_mcwpInfo.numWaypoints < MAX_WAYPOINTS) {
		m_mcwpInfo.numWaypoints++;
		m_mcwpInfo.waypoints[m_mcwpInfo.numWaypoints] = *pWay->getLocation();
		if (pWay->getNumLinks() > 0) {
			pWay = pWay->getLink(0);
		} else {
			pWay = NULL;
		}
		Vector2 dir(
			m_mcwpInfo.waypoints[m_mcwpInfo.numWaypoints].x - m_mcwpInfo.waypoints[m_mcwpInfo.numWaypoints-1].x,
			m_mcwpInfo.waypoints[m_mcwpInfo.numWaypoints].y - m_mcwpInfo.waypoints[m_mcwpInfo.numWaypoints-1].y);
		if (dir.Length() < MIN_DELTA) {
			if (pWay) {
				m_mcwpInfo.numWaypoints--;
			} else {
				m_mcwpInfo.waypoints[m_mcwpInfo.numWaypoints-1] = m_mcwpInfo.waypoints[m_mcwpInfo.numWaypoints];
				m_mcwpInfo.numWaypoints--;
			}
		}
	}
	setupWaypointPath(orient);
}

void W3DView::setupWaypointPath( Bool orient )
{
	m_mcwpInfo.curSegment = 1;
	m_mcwpInfo.curSegDistance = 0;
	m_mcwpInfo.totalDistance = 0;
	m_mcwpInfo.rollingAverageFrames = 1;
	Int i;
	Real angle = getAngle();
	for (i = 1; i < m_mcwpInfo.numWaypoints; i++) {
		Vector2 dir(
			m_mcwpInfo.waypoints[i+1].x - m_mcwpInfo.waypoints[i].x,
			m_mcwpInfo.waypoints[i+1].y - m_mcwpInfo.waypoints[i].y);
		m_mcwpInfo.waySegLength[i] = dir.Length();
		m_mcwpInfo.totalDistance += m_mcwpInfo.waySegLength[i];
		if (orient) {
			angle = WWMath::Acos(dir.X / m_mcwpInfo.waySegLength[i]);
			if (dir.Y < 0.0f)
				angle = -angle;
			angle -= PI / 2;
			normAngle(angle);
		}
		m_mcwpInfo.cameraAngle[i] = angle;
	}
	m_mcwpInfo.cameraAngle[1] = getAngle();
	m_mcwpInfo.cameraAngle[m_mcwpInfo.numWaypoints] = m_mcwpInfo.cameraAngle[m_mcwpInfo.numWaypoints-1];
	for (i = m_mcwpInfo.numWaypoints-1; i > 1; i--) {
		m_mcwpInfo.cameraAngle[i] = (m_mcwpInfo.cameraAngle[i] + m_mcwpInfo.cameraAngle[i-1]) / 2;
	}
	m_mcwpInfo.waySegLength[m_mcwpInfo.numWaypoints+1] = m_mcwpInfo.waySegLength[m_mcwpInfo.numWaypoints];

	if (m_mcwpInfo.totalDistance < 1.0f) {
		m_mcwpInfo.waySegLength[m_mcwpInfo.numWaypoints-1] += 1.0f - m_mcwpInfo.totalDistance;
		m_mcwpInfo.totalDistance = 1.0f;
	}

	Real curDistance = 0;
	Coord3D finalPos = m_mcwpInfo.waypoints[m_mcwpInfo.numWaypoints];
	Real newGround = TheTerrainLogic ? TheTerrainLogic->getGroundHeight(finalPos.x, finalPos.y) : 0.0f;
	for (i = 0; i <= m_mcwpInfo.numWaypoints + 1; i++) {
		Real factor2 = curDistance / m_mcwpInfo.totalDistance;
		Real factor1 = 1.0f - factor2;
		m_mcwpInfo.timeMultiplier[i] = m_timeMultiplier;
		m_mcwpInfo.groundHeight[i] = m_groundLevel * factor1 + newGround * factor2;
		curDistance += m_mcwpInfo.waySegLength[i];
	}

	// Extrapolate end padding
	m_mcwpInfo.waypoints[m_mcwpInfo.numWaypoints+1] = m_mcwpInfo.waypoints[m_mcwpInfo.numWaypoints];
	Coord3D cur  = m_mcwpInfo.waypoints[m_mcwpInfo.numWaypoints];
	Coord3D prev = m_mcwpInfo.waypoints[m_mcwpInfo.numWaypoints-1];
	m_mcwpInfo.waypoints[m_mcwpInfo.numWaypoints+1].x += cur.x - prev.x;
	m_mcwpInfo.waypoints[m_mcwpInfo.numWaypoints+1].y += cur.y - prev.y;
	m_mcwpInfo.cameraAngle[m_mcwpInfo.numWaypoints+1]  = m_mcwpInfo.cameraAngle[m_mcwpInfo.numWaypoints];
	m_mcwpInfo.groundHeight[m_mcwpInfo.numWaypoints+1] = newGround;

	// Extrapolate start padding
	cur  = m_mcwpInfo.waypoints[2];
	prev = m_mcwpInfo.waypoints[1];
	m_mcwpInfo.waypoints[0].x -= cur.x - prev.x;
	m_mcwpInfo.waypoints[0].y -= cur.y - prev.y;

	m_doingMoveCameraOnWaypointPath     = (m_mcwpInfo.numWaypoints > 1);
	m_CameraArrivedAtWaypointOnPathFlag = false;
	m_doingRotateCamera                 = false;

	m_mcwpInfo.elapsedTimeMilliseconds = 0;
	m_mcwpInfo.curShutter = m_mcwpInfo.shutter;
}

Bool W3DView::isCameraMovementFinished( void )
{
	return !m_doingMoveCameraOnWaypointPath && !m_doingRotateCamera
		&& !m_doingPitchCamera && !m_doingZoomCamera;
}

Bool W3DView::isCameraMovementAtWaypointAlongPath( void )
{
	Bool return_value = m_CameraArrivedAtWaypointOnPathFlag;
	m_CameraArrivedAtWaypointOnPathFlag = false;	// cleared by the poll, per retail
	return return_value;
}

void W3DView::resetCamera( const Coord3D *location, Int milliseconds, Real easeIn, Real easeOut )
{
	moveCameraTo(location, milliseconds, 0, false, easeIn, easeOut);
	m_mcwpInfo.cameraAngle[2] = 0.0; // default angle.
	m_angle = m_mcwpInfo.cameraAngle[0];

	// terrain height + desired height offset == cameraOffset * actual zoom
	// find best approximation of max terrain height we can see
	Real terrainHeightMax = getHeightAroundPos(location->x, location->y);
	Real desiredHeight = (terrainHeightMax + m_maxHeightAboveGround);
	Real desiredZoom = desiredHeight / m_cameraOffset.z;

	zoomCamera( desiredZoom, milliseconds, easeIn, easeOut );

	pitchCamera( 1.0, milliseconds, easeIn, easeOut );
}

void W3DView::rotateCamera( Real rotations, Int milliseconds, Real easeIn, Real easeOut )
{
	m_rcInfo.numHoldFrames = 0;
	m_rcInfo.trackObject = FALSE;

	if (milliseconds < 1) milliseconds = 1;
	m_rcInfo.numFrames = milliseconds/TheW3DFrameLengthInMsec;
	if (m_rcInfo.numFrames < 1) {
		m_rcInfo.numFrames = 1;
	}
	m_rcInfo.curFrame = 0;
	m_doingRotateCamera = true;
	m_rcInfo.angle.startAngle = m_angle;
	m_rcInfo.angle.endAngle = m_angle + 2*PI*rotations;
	m_rcInfo.startTimeMultiplier = m_timeMultiplier;
	m_rcInfo.endTimeMultiplier = m_timeMultiplier;
	m_rcInfo.ease.setEaseTimes(easeIn/milliseconds, easeOut/milliseconds);

	m_doingMoveCameraOnWaypointPath = false;
	m_CameraArrivedAtWaypointOnPathFlag = false;
}
void W3DView::rotateCameraTowardObject( ObjectID id, Int milliseconds, Int holdMilliseconds, Real easeIn, Real easeOut )
{
	m_rcInfo.trackObject = TRUE;
	if (holdMilliseconds < 1) holdMilliseconds = 0;
	m_rcInfo.numHoldFrames = holdMilliseconds/TheW3DFrameLengthInMsec;
	if (m_rcInfo.numHoldFrames < 1) {
		m_rcInfo.numHoldFrames = 0;
	}

	if (milliseconds < 1) milliseconds = 1;
	m_rcInfo.numFrames = milliseconds/TheW3DFrameLengthInMsec;
	if (m_rcInfo.numFrames < 1) {
		m_rcInfo.numFrames = 1;
	}
	m_rcInfo.curFrame = 0;
	m_doingRotateCamera = true;
	m_rcInfo.target.targetObjectID = id;
	m_rcInfo.startTimeMultiplier = m_timeMultiplier;
	m_rcInfo.endTimeMultiplier = m_timeMultiplier;
	m_rcInfo.ease.setEaseTimes(easeIn/milliseconds, easeOut/milliseconds);

	m_doingMoveCameraOnWaypointPath = false;
	m_CameraArrivedAtWaypointOnPathFlag = false;
}
void W3DView::rotateCameraTowardPosition( const Coord3D *pLoc, Int milliseconds, Real easeIn, Real easeOut, Bool reverseRotation )
{
	m_rcInfo.numHoldFrames = 0;
	m_rcInfo.trackObject = FALSE;

	if (milliseconds < 1) milliseconds = 1;
	m_rcInfo.numFrames = milliseconds/TheW3DFrameLengthInMsec;
	if (m_rcInfo.numFrames < 1) {
		m_rcInfo.numFrames = 1;
	}
	Coord3D curPos = *getPosition();
	Vector2 dir(pLoc->x-curPos.x, pLoc->y-curPos.y);
	const Real dirLength = dir.Length();
	if (dirLength < 0.1f) return;
	Real angle = WWMath::Acos(dir.X/dirLength);
	if (dir.Y < 0.0f) {
		angle = -angle;
	}
	// Default camera is rotated 90 degrees, so match.
	angle -= PI/2;
	normAngle(angle);

	if (reverseRotation) {
		if (m_angle < angle) {
			angle -= 2.0f*WWMATH_PI;
		} else {
			angle += 2.0f*WWMATH_PI;
		}
	}

	m_rcInfo.curFrame = 0;
	m_doingRotateCamera = true;
	m_rcInfo.angle.startAngle = m_angle;
	m_rcInfo.angle.endAngle = angle;
	m_rcInfo.startTimeMultiplier = m_timeMultiplier;
	m_rcInfo.endTimeMultiplier = m_timeMultiplier;
	m_rcInfo.ease.setEaseTimes(easeIn/milliseconds, easeOut/milliseconds);

	m_doingMoveCameraOnWaypointPath = false;
	m_CameraArrivedAtWaypointOnPathFlag = false;
}

void W3DView::cameraModFreezeAngle( void )
{
	if (m_doingRotateCamera) {
		if (m_rcInfo.trackObject) {
			m_rcInfo.target.targetObjectID = INVALID_ID;
		} else {
			m_rcInfo.angle.startAngle = m_rcInfo.angle.endAngle = m_angle; // Silly, but consistent.
		}
	}
	if (m_doingMoveCameraOnWaypointPath) {
		Int i;
		for (i = 0; i < m_mcwpInfo.numWaypoints; i++) {
			m_mcwpInfo.cameraAngle[i+1] = m_mcwpInfo.cameraAngle[0];
		}
	}
}
void W3DView::cameraModFinalZoom( Real finalZoom, Real easeIn, Real easeOut )
{
	if (m_doingRotateCamera)
	{
		Real terrainHeightMax = getHeightAroundPos(m_pos.x, m_pos.y);
		Real maxHeight = (terrainHeightMax + m_maxHeightAboveGround);
		Real maxZoom = maxHeight / m_cameraOffset.z;

		Real time = (m_rcInfo.numFrames + m_rcInfo.numHoldFrames - m_rcInfo.curFrame)*TheW3DFrameLengthInMsec;
		zoomCamera( finalZoom*maxZoom, time, time*easeIn, time*easeOut );
	}
	if (m_doingMoveCameraOnWaypointPath)
	{
		Coord3D pos = m_mcwpInfo.waypoints[m_mcwpInfo.numWaypoints];
		Real terrainHeightMax = getHeightAroundPos(pos.x, pos.y);
		Real maxHeight = (terrainHeightMax + m_maxHeightAboveGround);
		Real maxZoom = maxHeight / m_cameraOffset.z;

		Real time = m_mcwpInfo.totalTimeMilliseconds - m_mcwpInfo.elapsedTimeMilliseconds;
		zoomCamera( finalZoom*maxZoom, time, time*easeIn, time*easeOut );
	}
}
void W3DView::cameraModRollingAverage( Int framesToAverage )
{
	if (framesToAverage < 1) framesToAverage = 1;
	m_mcwpInfo.rollingAverageFrames = framesToAverage;
}
void W3DView::cameraModFinalTimeMultiplier( Int finalMultiplier )
{
	if (m_doingZoomCamera)
		m_zcInfo.endTimeMultiplier = finalMultiplier;
	if (m_doingPitchCamera)
		m_pcInfo.endTimeMultiplier = finalMultiplier;
	if (m_doingRotateCamera) {
		m_rcInfo.endTimeMultiplier = finalMultiplier;
	} else if (m_doingMoveCameraOnWaypointPath) {
		Int i;
		Real curDistance = 0;
		for (i = 0; i < m_mcwpInfo.numWaypoints; i++) {
			curDistance += m_mcwpInfo.waySegLength[i];
			Real factor2 = curDistance / m_mcwpInfo.totalDistance;
			Real factor1 = 1.0-factor2;
			m_mcwpInfo.timeMultiplier[i+1] = REAL_TO_INT_FLOOR(0.5+m_mcwpInfo.timeMultiplier[i+1]*factor1 + finalMultiplier*factor2);
		}
	} else {
		// If we are not doing a camera movement, just set the time.
		m_timeMultiplier = finalMultiplier;
	}
}
void W3DView::cameraModFinalPitch( Real finalPitch, Real easeIn, Real easeOut )
{
	if (m_doingRotateCamera) {
		Real time = (m_rcInfo.numFrames + m_rcInfo.numHoldFrames - m_rcInfo.curFrame)*TheW3DFrameLengthInMsec;
		pitchCamera( finalPitch, time, time*easeIn, time*easeOut );
	}
	if (m_doingMoveCameraOnWaypointPath) {
		Real time = m_mcwpInfo.totalTimeMilliseconds - m_mcwpInfo.elapsedTimeMilliseconds;
		pitchCamera( finalPitch, time, time*easeIn, time*easeOut );
	}
}
void W3DView::cameraModLookToward( Coord3D *pLoc )
{
	if (m_doingRotateCamera)
		return;
	if (!m_doingMoveCameraOnWaypointPath)
		return;
	Int i;
	for (i = 2; i <= m_mcwpInfo.numWaypoints; i++) {
		Coord3D start, mid, end;
		Real factor = 0.5f;
		start = m_mcwpInfo.waypoints[i-1];
		start.x += m_mcwpInfo.waypoints[i].x;
		start.y += m_mcwpInfo.waypoints[i].y;
		start.x /= 2; start.y /= 2;
		mid = m_mcwpInfo.waypoints[i];
		end = m_mcwpInfo.waypoints[i];
		end.x += m_mcwpInfo.waypoints[i+1].x;
		end.y += m_mcwpInfo.waypoints[i+1].y;
		end.x /= 2; end.y /= 2;
		Coord3D result = start;
		result.x += factor * (end.x - start.x);
		result.y += factor * (end.y - start.y);
		result.x += (1-factor) * factor * (mid.x - end.x + mid.x - start.x);
		result.y += (1-factor) * factor * (mid.y - end.y + mid.y - start.y);
		result.z = 0;
		Vector2 dir(pLoc->x - result.x, pLoc->y - result.y);
		const Real dirLength = dir.Length();
		if (dirLength < 0.1f) continue;
		Real angle = WWMath::Acos(dir.X / dirLength);
		if (dir.Y < 0.0f)
			angle = -angle;
		angle -= PI / 2;
		normAngle(angle);
		m_mcwpInfo.cameraAngle[i] = angle;
	}
	if (m_mcwpInfo.totalTimeMilliseconds == 1) {
		moveAlongWaypointPath(1);
		m_doingMoveCameraOnWaypointPath = true;
		m_CameraArrivedAtWaypointOnPathFlag = false;
	}
}

void W3DView::cameraModFinalLookToward( Coord3D *pLoc )
{
	if (m_doingRotateCamera)
		return;
	if (!m_doingMoveCameraOnWaypointPath)
		return;
	Int i;
	Int min = m_mcwpInfo.numWaypoints - 1;
	if (min < 2) min = 2;
	for (i = min; i <= m_mcwpInfo.numWaypoints; i++) {
		Coord3D start, mid, end;
		Real factor = 0.5f;
		start = m_mcwpInfo.waypoints[i-1];
		start.x += m_mcwpInfo.waypoints[i].x;
		start.y += m_mcwpInfo.waypoints[i].y;
		start.x /= 2; start.y /= 2;
		mid = m_mcwpInfo.waypoints[i];
		end = m_mcwpInfo.waypoints[i];
		end.x += m_mcwpInfo.waypoints[i+1].x;
		end.y += m_mcwpInfo.waypoints[i+1].y;
		end.x /= 2; end.y /= 2;
		Coord3D result = start;
		result.x += factor * (end.x - start.x);
		result.y += factor * (end.y - start.y);
		result.x += (1-factor) * factor * (mid.x - end.x + mid.x - start.x);
		result.y += (1-factor) * factor * (mid.y - end.y + mid.y - start.y);
		result.z = 0;
		Vector2 dir(pLoc->x - result.x, pLoc->y - result.y);
		const Real dirLength = dir.Length();
		if (dirLength < 0.1f) continue;
		Real angle = WWMath::Acos(dir.X / dirLength);
		if (dir.Y < 0.0f)
			angle = -angle;
		angle -= PI / 2;
		normAngle(angle);
		if (i == m_mcwpInfo.numWaypoints) {
			m_mcwpInfo.cameraAngle[i] = angle;
		} else {
			Real deltaAngle = angle - m_mcwpInfo.cameraAngle[i];
			normAngle(deltaAngle);
			angle = m_mcwpInfo.cameraAngle[i] + deltaAngle / 2;
			normAngle(angle);
			m_mcwpInfo.cameraAngle[i] = angle;
		}
	}
}
void W3DView::cameraModFinalMoveTo( Coord3D *pLoc )
{
	if (m_doingRotateCamera) {
		return; // Does not apply to rotate about a point.
	}
	if (m_doingMoveCameraOnWaypointPath) {
		Int i;
		Coord3D start, delta;
		start = m_mcwpInfo.waypoints[m_mcwpInfo.numWaypoints];
		delta.x = pLoc->x - start.x;
		delta.y = pLoc->y - start.y;
		for (i = 2; i <= m_mcwpInfo.numWaypoints; i++) {
			Coord3D result = m_mcwpInfo.waypoints[i];
			result.x += delta.x;
			result.y += delta.y;
			m_mcwpInfo.waypoints[i] = result;
		}
	}
}

void W3DView::cameraEnableSlaveMode( const AsciiString &thingtemplateName, const AsciiString &boneName ) { /* TODO: BGFX port */ }
void W3DView::cameraDisableSlaveMode( void ) { /* TODO: BGFX port */ }
void W3DView::cameraEnableRealZoomMode( void ) { /* TODO: BGFX port */ }
void W3DView::cameraDisableRealZoomMode( void ) { /* TODO: BGFX port */ }

void W3DView::Add_Camera_Shake( const Coord3D &position, float radius, float duration, float power ) { /* TODO: BGFX port */ }

void W3DView::setDefaultView( Real pitch, Real angle, Real maxHeight )
{
	// MDC - we no longer want to rotate maps (design made all of them right to begin with)
	//	m_defaultAngle = angle * M_PI/180.0f;
	// NB: writes the *default* pitch, not the live pitch/angle, and scales maxHeight
	// by the global cap instead of assigning it raw. Assigning it raw left
	// m_maxHeightAboveGround at 1.0, since InGameUI calls this with maxHeight 1.0.
	m_defaultPitchAngle = pitch;
	m_maxHeightAboveGround = TheGlobalData->m_maxCameraHeight * maxHeight;
	if (m_minHeightAboveGround > m_maxHeightAboveGround)
		m_maxHeightAboveGround = m_minHeightAboveGround;
}

void W3DView::zoomCamera( Real finalZoom, Int milliseconds, Real easeIn, Real easeOut )
{
	if (milliseconds < 1) milliseconds = 1;
	m_zcInfo.numFrames = milliseconds/TheW3DFrameLengthInMsec;
	if (m_zcInfo.numFrames < 1) {
		m_zcInfo.numFrames = 1;
	}
	m_zcInfo.curFrame = 0;
	m_doingZoomCamera = TRUE;
	m_zcInfo.startZoom = m_zoom;
	m_zcInfo.endZoom = finalZoom;
	m_zcInfo.ease.setEaseTimes(easeIn/milliseconds, easeOut/milliseconds);
}
void W3DView::pitchCamera( Real finalPitch, Int milliseconds, Real easeIn, Real easeOut )
{
	if (milliseconds < 1) milliseconds = 1;
	m_pcInfo.numFrames = milliseconds/TheW3DFrameLengthInMsec;
	if (m_pcInfo.numFrames < 1) {
		m_pcInfo.numFrames = 1;
	}
	m_pcInfo.curFrame = 0;
	m_doingPitchCamera = TRUE;
	m_pcInfo.startPitch = m_FXPitch;
	m_pcInfo.endPitch = finalPitch;
	m_pcInfo.ease.setEaseTimes(easeIn/milliseconds, easeOut/milliseconds);
}

void W3DView::setHeightAboveGround( Real z )
{
	m_heightAboveGround = z;

	// if our zoom is limited, we will stay within a predefined distance from the terrain
	if( m_zoomLimited )
	{
		if (m_heightAboveGround < m_minHeightAboveGround)
			m_heightAboveGround = m_minHeightAboveGround;

		if (m_heightAboveGround > m_maxHeightAboveGround)
			m_heightAboveGround = m_maxHeightAboveGround;
	}  // end if

	m_doingMoveCameraOnWaypointPath = false;
	m_CameraArrivedAtWaypointOnPathFlag = false;
	m_doingRotateCamera = false;
	m_doingPitchCamera = false;
	m_doingZoomCamera = false;
	m_doingScriptedCameraLock = false;
	m_cameraConstraintValid = false; // recalc it.
	setCameraTransform();
}
void W3DView::setZoom( Real z )
{
	m_zoom = z;

	if (m_zoom < m_minZoom)
		m_zoom = m_minZoom;

	if (m_zoom > m_maxZoom)
		m_zoom = m_maxZoom;

	m_doingMoveCameraOnWaypointPath = false;
	m_CameraArrivedAtWaypointOnPathFlag = false;
	m_doingRotateCamera = false;
	m_doingPitchCamera = false;
	m_doingZoomCamera = false;
	m_doingScriptedCameraLock = false;
	m_cameraConstraintValid = false; // recalc it.
	setCameraTransform();
}
void W3DView::setZoomToDefault( void )
{
	// default zoom has to be max, otherwise players will just zoom to max always

	// terrain height + desired height offset == cameraOffset * actual zoom
	// find best approximation of max terrain height we can see
	Real terrainHeightMax = getHeightAroundPos(m_pos.x, m_pos.y);

	Real desiredHeight = (terrainHeightMax + m_maxHeightAboveGround);
	Real desiredZoom = desiredHeight / m_cameraOffset.z;

	m_zoom = desiredZoom;
	// This assignment is the whole point: m_heightAboveGround has no other
	// initialiser anywhere in the tree, so without it the height servo in
	// updateCameraMovements runs against a setpoint of 0 and drives the camera
	// down onto the terrain.
	m_heightAboveGround = m_maxHeightAboveGround;

	m_doingMoveCameraOnWaypointPath = false;
	m_CameraArrivedAtWaypointOnPathFlag = false;
	m_doingRotateCamera = false;
	m_doingPitchCamera = false;
	m_doingZoomCamera = false;
	m_doingScriptedCameraLock = false;
	m_cameraConstraintValid = false; // recalc it.
	setCameraTransform();
}

