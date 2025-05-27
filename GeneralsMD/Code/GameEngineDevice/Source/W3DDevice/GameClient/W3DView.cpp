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
	m_cameraOffset.z = TheGlobalData->m_cameraHeight;
	m_cameraOffset.y = -(m_cameraOffset.z / tan(TheGlobalData->m_cameraPitch * (PI / 180.0)));
	m_cameraOffset.x = -(m_cameraOffset.y * tan(TheGlobalData->m_cameraYaw * (PI / 180.0)));

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


	Real factor = 1.0 - (groundLevel/sourcePos.Z );

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
	//DEBUG_LOG(("zoom %f, SourceZ %f, posZ %f, groundLevel %f CamOffZ %f\n",
	//			zoom, sourcePos.Z, pos.z, groundLevel,m_cameraOffset.z));

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
	m_cameraOffset.z = TheGlobalData->m_cameraHeight;
	m_cameraOffset.y = -(m_cameraOffset.z / tan(TheGlobalData->m_cameraPitch * (PI / 180.0)));
	m_cameraOffset.x = -(m_cameraOffset.y * tan(TheGlobalData->m_cameraYaw * (PI / 180.0)));

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


	Real factor = 1.0 - (groundLevel/sourcePos.Z );

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
	//DEBUG_LOG(("zoom %f, SourceZ %f, posZ %f, groundLevel %f CamOffZ %f\n",
	//			zoom, sourcePos.Z, pos.z, groundLevel,m_cameraOffset.z));

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