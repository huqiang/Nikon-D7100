//================================================================================================
// Copyright Nikon Corporation - All rights reserved
//
// View this file in a non-proportional font, tabs = 3
//================================================================================================

#include	<stdlib.h>
#include	<stdio.h>
#include	"maid3.h"
#include	"maid3d1.h"
#include	"CtrlSample.h"

LPMAIDEntryPointProc	g_pMAIDEntryPoint = NULL;
UCHAR	g_bFileRemoved = false;
ULONG	g_ulCameraType = 0;	// CameraType

#if defined( _WIN32 )
	HINSTANCE	g_hInstModule = NULL;
#elif defined(__APPLE__)
	CFBundleRef gBundle = NULL;
#endif

//------------------------------------------------------------------------------------------------------------------------------------
//
int main()
{
#if defined( _WIN32 )
	char	ModulePath[MAX_PATH];
#elif defined(__APPLE__)
	FSRef ModuleRef;
#endif
	LPRefObj	pRefMod = NULL, pRefSrc = NULL, RefItm = NULL, pRefDat = NULL;
	char	buf[256];
	ULONG	ulModID = 0, ulSrcID = 0;
	UWORD	wSel;
	BOOL	bRet;

	// Search for a Module-file like "Type0010.md3".
#if defined( _WIN32 )
	bRet = Search_Module( ModulePath );
#elif defined(__APPLE__)
	bRet = Search_Module( &ModuleRef );
#endif
	if ( bRet == false ) {
		puts( "\"Type0010 Module\" is not found.\n" );
		return -1;
	}

	// Load the Module-file.
#if defined( _WIN32 )
	bRet = Load_Module( ModulePath );
#elif defined(__APPLE__)
	bRet = Load_Module( &ModuleRef );
#endif
	if ( bRet == false ) {
		puts( "Failed in loading \"Type0010 Module\".\n" );
		return -1;
	}

	// Allocate memory for reference to Module object.
	pRefMod = (LPRefObj)malloc(sizeof(RefObj));
	if ( pRefMod == NULL ) {
		puts( "There is not enough memory." );
		return -1;
	}
	InitRefObj( pRefMod );

	// Allocate memory for Module object.
	pRefMod->pObject = (LPNkMAIDObject)malloc(sizeof(NkMAIDObject));
	if ( pRefMod->pObject == NULL ) {
		puts( "There is not enough memory." );
		if ( pRefMod != NULL )	free( pRefMod );
		return -1;
	}

	//	Open Module object
	pRefMod->pObject->refClient = (NKREF)pRefMod;
	bRet = Command_Open(	NULL,					// When Module_Object will be opend, "pParentObj" is "NULL".
								pRefMod->pObject,	// Pointer to Module_Object 
								ulModID );			// Module object ID set by Client
	if ( bRet == false ) {
		puts( "Module object can't be opened.\n" );
		if ( pRefMod->pObject != NULL )	free( pRefMod->pObject );
		if ( pRefMod != NULL )	free( pRefMod );
		return -1;
	}

	//	Enumerate Capabilities that the Module has.
	bRet = EnumCapabilities( pRefMod->pObject, &(pRefMod->ulCapCount), &(pRefMod->pCapArray), NULL, NULL );
	if ( bRet == false ) {
		puts( "Failed in enumeration of capabilities." );
		if ( pRefMod->pObject != NULL )	free( pRefMod->pObject );
		if ( pRefMod != NULL )	free( pRefMod );
		return -1;
	}

	//	Set the callback functions(ProgressProc, EventProc and UIRequestProc).
	bRet = SetProc( pRefMod );
	if ( bRet == false ) {
		puts( "Failed in setting a call back function." );
		if ( pRefMod->pObject != NULL )	free( pRefMod->pObject );
		if ( pRefMod != NULL )	free( pRefMod );
		return -1;
	}

	//	Set the kNkMAIDCapability_ModuleMode.
	if( CheckCapabilityOperation( pRefMod, kNkMAIDCapability_ModuleMode, kNkMAIDCapOperation_Set )  ){
		bRet = Command_CapSet( pRefMod->pObject, kNkMAIDCapability_ModuleMode, kNkMAIDDataType_Unsigned, 
										(NKPARAM)kNkMAIDModuleMode_Controller, NULL, NULL);
		if ( bRet == false ) {
			puts( "Failed in setting kNkMAIDCapability_ModuleMode." );
			return -1;
		}
	}

	// Module Command Loop
	do {
		printf( "\nSelect (1-6, 0)\n" );
		printf( " 1. Select Device            2. AsyncRate                3. IsAlive\n" );
		printf( " 4. Name                     5. ModuleType               6. Version\n" );
		printf( " 0. Exit\n>" );
		scanf( "%s", buf );
		wSel = atoi( buf );

		switch( wSel )
		{
			case 1:// Children
				// Select Device
				ulSrcID = 0;	// 0 means Device count is zero. 
				bRet = SelectSource( pRefMod, &ulSrcID );
				if ( bRet == false ) break;
				if( ulSrcID > 0 )
					bRet = SourceCommandLoop( pRefMod, ulSrcID );
				break;
			case 2:// AsyncRate
				bRet = SetUnsignedCapability( pRefMod, kNkMAIDCapability_AsyncRate );
				break;
			case 3:// IsAlive
				bRet = SetBoolCapability( pRefMod, kNkMAIDCapability_IsAlive );
				break;
			case 4:// Name
				bRet = SetStringCapability( pRefMod, kNkMAIDCapability_Name );
				break;
			case 5:// ModuleType
				bRet = SetUnsignedCapability( pRefMod, kNkMAIDCapability_ModuleType );
				break;
			case 6:// Version
				bRet = SetUnsignedCapability( pRefMod, kNkMAIDCapability_Version );
				break;
			default:
				wSel = 0;
		}
		if ( bRet == false ) {
			printf( "An Error occured. Enter '0' to exit.\n>" );
			scanf( "%s", buf );
			bRet = true;
		}
	} while( wSel > 0 && bRet == true );

	// Close Module_Object
	bRet = Close_Module( pRefMod );
	if ( bRet == false )
		puts( "Module object can not be closed.\n" );

	// Unload Module
#if defined( _WIN32 )
	FreeLibrary( g_hInstModule );
	g_hInstModule = NULL;
#elif defined(__APPLE__)
	if (gBundle != NULL)
	{
		CFBundleUnloadExecutable(gBundle);
		CFRelease(gBundle);
		gBundle = NULL;
	}
#endif

	// Free memory blocks allocated in this function.
	if ( pRefMod->pObject != NULL )	free( pRefMod->pObject );
	if ( pRefMod != NULL )	free( pRefMod );
	
	puts( "This sample program has terminated.\n" );
	return 0;
}
//------------------------------------------------------------------------------------------------------------------------------------
//
BOOL SourceCommandLoop( LPRefObj pRefMod, ULONG ulSrcID )
{
	LPRefObj	pRefSrc = NULL;
	char	buf[256];
	ULONG	ulItemID = 0;
	UWORD	wSel;
	BOOL	bRet = true;

	pRefSrc = GetRefChildPtr_ID( pRefMod, ulSrcID );
	if ( pRefSrc == NULL ) {
		// Create Source object and RefSrc structure.
		if ( AddChild( pRefMod, ulSrcID ) == true ) {
			printf("Source object is opened.\n");
		} else {
			printf("Source object can't be opened.\n");
			return false;
		}
		pRefSrc = GetRefChildPtr_ID( pRefMod, ulSrcID );
	}

	// Get CameraType
	Command_CapGet( pRefSrc->pObject, kNkMAIDCapability_CameraType, kNkMAIDDataType_UnsignedPtr, (NKPARAM)&g_ulCameraType, NULL, NULL );

	// command loop
	do {
		printf( "\nSelect (1-10, 0)\n" );
		printf( " 1. Select Item Object       2. Camera settings(1)       3. Camera settings(2)\n" );
		printf( " 4. Shooting Menu            5. Custom Menu              6. Async\n" );
		printf( " 7. Autofocus                8. Capture                  9. TerminateCapture\n" );
		printf( "10. PreCapture\n" );
		printf( " 0. Exit\n>" );
		scanf( "%s", buf );
		wSel = atoi( buf );

		switch( wSel )
		{
			case 1:// Children
				// Select Item  Object
				ulItemID = 0;
				bRet = SelectItem( pRefSrc, &ulItemID );
				if( bRet == true && ulItemID > 0 )
					bRet = ItemCommandLoop( pRefSrc, ulItemID );
				break;
			case 2:// Camera setting 1
				bRet = SetUpCamera1( pRefSrc );
				break;
			case 3:// Camera setting 2
				bRet = SetUpCamera2( pRefSrc );
				break;
			case 4:// Shooting Menu
				bRet = SetShootingMenu( pRefSrc );
				break;
			case 5:// CustomSetting Menu
				bRet = SetCustomSettings( pRefSrc );
				break;
			case 6:// Async
				bRet = Command_Async( pRefMod->pObject );
				break;
			case 7:// AutoFocus
				bRet = IssueProcess( pRefSrc, kNkMAIDCapability_AutoFocus );
				break;
			case 8:// Capture
				bRet = IssueProcess( pRefSrc, kNkMAIDCapability_Capture );
				Command_Async( pRefSrc->pObject );
				break;
			case 9:// TerminateCapture
				bRet = TerminateCaptureCapability( pRefSrc );
				break;
			case 10:// PreCapture
				bRet = IssueProcess( pRefSrc, kNkMAIDCapability_PreCapture );
				break;
			default:
				wSel = 0;
		}
		if ( bRet == false ) {
			printf( "An Error occured. Enter '0' to exit.\n>" );
			scanf( "%s", buf );
			bRet = true;
		}
		WaitEvent();
	} while( wSel > 0 );

// Close Source_Object
	bRet = RemoveChild( pRefMod, ulSrcID );

	return true;
}
//------------------------------------------------------------------------------------------------------------------------------------
//
BOOL SetUpCamera1( LPRefObj pRefSrc ) 
{
	char	buf[256];
	UWORD	wSel;
	BOOL	bRet = true;

	do {
		// Wait for selection by user
		printf( "\nSelect the item you want to set up\n" );
		printf( " 1. IsAlive                  2. Name                      3. Interface\n" );
		printf( " 4. DataTypes                5. BatteryLevel              6. FlashMode\n" );
		printf( " 7. LockFocus                8. LockExposure              9. ExposureStatus\n" );
		printf( "10. ExposureMode            11. ShutterSpeed             12. Aperture\n" );
		printf( "13. FlexibleProgram         14. ExposureComp             15. MeteringMode\n" );
		printf( "16. FocusMode               17. FocusAreaMode            18. FocusPreferredArea\n" );
		printf( "19. FocalLength             20. ClockDateTime\n" );
		printf( " 0. Exit\n>" );
		scanf( "%s", buf );
		wSel = atoi( buf );

		switch( wSel )
		{
			case 1:// IsAlive
				bRet = SetBoolCapability( pRefSrc, kNkMAIDCapability_IsAlive );
				break;
			case 2:// Name
				bRet = SetStringCapability( pRefSrc, kNkMAIDCapability_Name );
				break;
			case 3:// Interface
				bRet = SetStringCapability( pRefSrc, kNkMAIDCapability_Interface );
				break;
			case 4:// DataTypes
				bRet = SetUnsignedCapability( pRefSrc, kNkMAIDCapability_DataTypes );
				break;
			case 5:// BatteryLevel
				bRet = SetIntegerCapability( pRefSrc, kNkMAIDCapability_BatteryLevel );
				break;
			case 6:// FlashMode
				bRet = SetEnumCapability( pRefSrc, kNkMAIDCapability_FlashMode );
				break;
			case 7:// LockFocus
				bRet = SetBoolCapability( pRefSrc, kNkMAIDCapability_LockFocus );
				break;
			case 8:// LockExposure
				bRet = SetBoolCapability( pRefSrc, kNkMAIDCapability_LockExposure );
				break;
			case 9:// ExposureStatus
				bRet = SetFloatCapability( pRefSrc, kNkMAIDCapability_ExposureStatus );
				break;
			case 10:// ExposureMode
				bRet = SetEnumCapability( pRefSrc, kNkMAIDCapability_ExposureMode );
				break;
			case 11:// ShutterSpeed(Exposure Time)
				bRet = SetEnumCapability( pRefSrc, kNkMAIDCapability_ShutterSpeed );
				break;
			case 12:// Aperture(F Number)
				bRet = SetEnumCapability( pRefSrc, kNkMAIDCapability_Aperture );
				break;
			case 13:// FlexibleProgram
				bRet = SetRangeCapability( pRefSrc, kNkMAIDCapability_FlexibleProgram );
				break;
			case 14:// ExposureCompensation
				bRet = SetRangeCapability( pRefSrc, kNkMAIDCapability_ExposureComp );
				break;
			case 15:// MeteringMode
				bRet = SetUnsignedCapability( pRefSrc, kNkMAIDCapability_MeteringMode );
				break;
			case 16:// FocusMode
				bRet = SetUnsignedCapability( pRefSrc, kNkMAIDCapability_FocusMode );
				break;
			case 17:// FocusAreaMode
				bRet = SetEnumCapability( pRefSrc, kNkMAIDCapability_FocusAreaMode );
				break;
			case 18:// FocusPreferredArea
				bRet = SetEnumCapability( pRefSrc, kNkMAIDCapability_FocusPreferredArea );
				break;
			case 19:// FocalLength
				bRet = SetFloatCapability( pRefSrc, kNkMAIDCapability_FocalLength );
				break;
			case 20:// ClockDateTime
				bRet = SetDateTimeCapability( pRefSrc, kNkMAIDCapability_ClockDateTime );
				break;
			default:
				wSel = 0;
				break;
		}
		if ( bRet == false ) {
			printf( "An Error occured. Enter '0' to exit.\n>" );
			scanf( "%s", buf );
			bRet = true;
		}
	} while( wSel != 0 );

	return bRet;
}
//------------------------------------------------------------------------------------------------------------------------------------
//
BOOL SetUpCamera2( LPRefObj pRefSrc ) 
{
	char	buf[256];
	UWORD	wSel;
	BOOL bRet = true;

	do {
		// Wait for selection by user
		printf( "\nSelect the item you want to set up\n" );
		printf( " 1. LockCamera             2. LensInfo                  3. UserComment\n" );
		printf( " 4. EnableComment          5. IsoControl                6. NoiseReduction\n" );
		printf( " 0. Exit\n>" );
		scanf( "%s", buf );
		wSel = atoi( buf );

		switch( wSel )
		{
			case 1:// LockCamera
				bRet = SetBoolCapability( pRefSrc, kNkMAIDCapability_LockCamera );
				break;
			case 2:// LensInfo
				bRet = SetStringCapability( pRefSrc, kNkMAIDCapability_LensInfo );
				break;
			case 3:// UserComment
				bRet = SetStringCapability( pRefSrc, kNkMAIDCapability_UserComment );
				break;
			case 4:// EnableComment
				bRet = SetBoolCapability( pRefSrc, kNkMAIDCapability_EnableComment );
				break;
			case 5:// IsoControl 
				bRet = SetBoolCapability( pRefSrc, kNkMAIDCapability_IsoControl );
				break;
			case 6:// NoiseReduction 
				bRet = SetBoolCapability( pRefSrc, kNkMAIDCapability_NoiseReduction );
				break;
			default:
				wSel = 0;
				break;
		}
		if ( bRet == false ) {
			printf( "An Error occured. Enter '0' to exit.\n>" );
			scanf( "%s", buf );
			bRet = true;
		}
	} while( wSel != 0 );

	return bRet;
}
//------------------------------------------------------------------------------------------------------------------------------------
//
BOOL SetShootingMenu( LPRefObj pRefSrc ) 
{
	char	buf[256];
	UWORD	wSel;
	ULONG ulPresetNumber = 0;
	BOOL bRet = true;

	do {
		// Wait for selection by user
		printf( "\nSelect the item you want to set up\n" );
		printf( " 1. CompressionLevel         2. ImageSize               3. WBMode\n" );
		printf( " 4. Sensitivity              5. WBTuneAuto              6. WBTuneIncandescent\n" );
		printf( " 7. WBFluorescentType        8. WBTuneFluorescent       9. WBTuneSunny\n" );
		printf( "10. WBTuneFlash             11. WBTuneShade            12. WBTuneCloudy\n" );
		printf( "13. WBPresetData            14. PictureControl         15. PictureControlData\n" );
		printf( "16. GetPicCtrlInfo          17. DeleteCustomPicCtrl    18. LiveViewProhibit\n" );
		printf( "19. LiveViewStatus          20. LiveViewImageZoomRate  21. GetLiveViewImage\n" );
		printf( "22. LiveViewImageSize       23. MovRecInCardProhibit   24. MovRecInCardStatus\n" );
		printf( "25. SpotWBMode              26. SpotWBMeasure          27. SpotWBChangeArea\n" );
		printf( "28. SpotWBResultDispEnd      0. Exit\n>" );
		scanf( "%s", buf );
		wSel = atoi( buf );

		switch( wSel )
		{
			case 1:// Compression Level
				bRet = SetEnumCapability( pRefSrc, kNkMAIDCapability_CompressionLevel );
				break;
			case 2:// ImageSize
				bRet = SetEnumCapability( pRefSrc, kNkMAIDCapability_ImageSize );
				break;
			case 3:// WBMode
				bRet = SetEnumCapability( pRefSrc, kNkMAIDCapability_WBMode );
				break;
			case 4:// Sensitivity
				bRet = SetEnumCapability( pRefSrc, kNkMAIDCapability_Sensitivity );
				break;
			case 5:// WBTuneAuto
				bRet = SetRangeCapability( pRefSrc, kNkMAIDCapability_WBTuneAuto );
				break;
			case 6:// WBTuneIncandescent
				bRet = SetRangeCapability( pRefSrc, kNkMAIDCapability_WBTuneIncandescent );
				break;
			case 7:// WBFluorescentType
				bRet = SetUnsignedCapability( pRefSrc, kNkMAIDCapability_WBFluorescentType );
				break;
			case 8:// WBTuneFluorescent
				bRet = SetRangeCapability( pRefSrc, kNkMAIDCapability_WBTuneFluorescent );
				break;
			case 9:// WBTuneSunny 
				bRet = SetRangeCapability( pRefSrc, kNkMAIDCapability_WBTuneSunny );
				break;
			case 10:// WBTuneFlash
				bRet = SetRangeCapability( pRefSrc, kNkMAIDCapability_WBTuneFlash );
				break;
			case 11:// WBTuneShade
				bRet = SetRangeCapability( pRefSrc, kNkMAIDCapability_WBTuneShade );
				break;
			case 12:// WBTuneCloudy
				bRet = SetRangeCapability( pRefSrc, kNkMAIDCapability_WBTuneCloudy );
				break;
			case 13:// WBPresetData
				bRet = SetWBPresetDataCapability( pRefSrc );
				break;
			case 14:// PictureControl
				bRet = SetEnumCapability( pRefSrc, kNkMAIDCapability_PictureControl );
				break;
			case 15:// PictureControlData
				bRet = PictureControlDataCapability( pRefSrc );
				break;
			case 16:// GetPicCtrlInfo 
				bRet = GetPictureControlInfoCapability( pRefSrc );
				break;
			case 17:// DeleteCustomPictureControl
				bRet = DeleteCustomPictureControlCapability( pRefSrc );
				break;
			case 18:// LiveViewProhibit
				bRet = SetUnsignedCapability( pRefSrc, kNkMAIDCapability_LiveViewProhibit );
				break;
			case 19:// LiveViewStatus
				bRet = SetUnsignedCapability( pRefSrc, kNkMAIDCapability_LiveViewStatus );
				break;
			case 20:// LiveViewImageZoomRate 
				bRet = SetEnumCapability( pRefSrc, kNkMAIDCapability_LiveViewImageZoomRate );
				break;
			case 21:// GetLiveViewImage
				bRet = GetLiveViewImageCapability( pRefSrc );
				break;
			case 22:// LiveViewImageSize
				bRet = SetEnumCapability( pRefSrc, kNkMAIDCapability_LiveViewImageSize );
				break;
			case 23:// MovRecInCardProhibit
				bRet = SetUnsignedCapability( pRefSrc, kNkMAIDCapability_MovRecInCardProhibit );
				break;
			case 24:// MovRecInCardStatus
				bRet = SetUnsignedCapability( pRefSrc, kNkMAIDCapability_MovRecInCardStatus );
				break;
			case 25:// SpotWBMode
				bRet = SetUnsignedCapability( pRefSrc, kNkMAIDCapability_SpotWBMode );
				break;
			case 26:// SpotWBMeasure
				bRet = IssueProcess( pRefSrc, kNkMAIDCapability_SpotWBMeasure );
				break;
			case 27:// SpotWBChangeArea
				bRet = SetPointCapability( pRefSrc, kNkMAIDCapability_SpotWBChangeArea );
				break;
			case 28:// SpotWBResultDispEnd
				bRet = IssueProcess( pRefSrc, kNkMAIDCapability_SpotWBResultDispEnd );
				break;

			default:
				wSel = 0;
				break;
		}
		if ( bRet == false ) {
			printf( "An Error occured. Enter '0' to exit.\n>" );
			scanf( "%s", buf );
			bRet = true;
		}
	} while( wSel != 0 );

	return bRet;
}
//------------------------------------------------------------------------------------------------------------------------------------
//
BOOL SetCustomSettings( LPRefObj pRefSrc ) 
{
	char	buf[256];
	UWORD	wSel;
	BOOL bRet = true;

	do {
		// Wait for selection by user
		printf( "\nSelect a Custom Setting\n" );
		printf( " 1. EVInterval              2. BracketingVary \n" );
		printf( " 0. Exit\n>" );
		scanf( "%s", buf );
		wSel = atoi( buf );

		switch( wSel )
		{
			case 1:// EVInterval
				bRet = SetEnumCapability( pRefSrc, kNkMAIDCapability_EVInterval );
				break;
			case 2:// BracketingVary
				bRet = SetEnumCapability( pRefSrc, kNkMAIDCapability_BracketingVary );
				break;
			default:
				wSel = 0;
				break;
		}
		if ( bRet == false ) {
			printf( "An Error occured. Enter '0' to exit.\n>" );
			scanf( "%s", buf );
			bRet = true;
		}
	} while( wSel != 0 );

	return bRet;
}
//------------------------------------------------------------------------------------------------------------------------------------
//
BOOL ItemCommandLoop( LPRefObj pRefSrc, ULONG ulItmID )
{
	ULONG	ulDataType = 0;
	LPRefObj	pRefItm = NULL;
	char	buf[256];
	UWORD	wSel;
	BOOL	bRet = true;

	pRefItm = GetRefChildPtr_ID( pRefSrc, ulItmID );
	if ( pRefItm == NULL ) {
		// Create Item object and RefSrc structure.
		if ( AddChild( pRefSrc, ulItmID ) == true ) {
			printf("Item object is opened.\n");
		} else {
			printf("Item object can't be opened.\n");
			return false;
		}
		pRefItm = GetRefChildPtr_ID( pRefSrc, ulItmID );
	}

	// command loop
	do {
	
		printf( "\nSelect (1-7, 0)\n" );
		printf( " 1. Select Data Object       2. Delete                   3. IsAlive\n" );
		printf( " 4. Name                     5. DataTypes                6. DateTime\n" );
		printf( " 7. StoredBytes\n" );
		printf( " 0. Exit\n>" );
		scanf( "%s", buf );
		wSel = atoi( buf );

		switch( wSel )
		{
			case 1:// Show Children
				// Select Data Object
				ulDataType = 0;
				bRet = SelectData( pRefItm, &ulDataType );
				if ( bRet == false )	return false;
				if ( ulDataType == kNkMAIDDataObjType_Image )
				{
					// reset file removed flag
					g_bFileRemoved = false;
					bRet = ImageCommandLoop( pRefItm, ulDataType );
					// If the image data was stored in DRAM, the item has been removed after reading image.
					if ( g_bFileRemoved ) {
						RemoveChild( pRefSrc, ulItmID );
						pRefItm = NULL;
					}
				}
				else if ( ulDataType == kNkMAIDDataObjType_Video )
				{
					// reset file removed flag
					g_bFileRemoved = false;
					bRet = MovieCommandLoop( pRefItm, ulDataType );
					if ( g_bFileRemoved ) {
						RemoveChild( pRefSrc, ulItmID );
						pRefItm = NULL;
					}
				}
				else if ( ulDataType == kNkMAIDDataObjType_Thumbnail )
				{
					bRet = ThumbnailCommandLoop( pRefItm, ulDataType );
				}
				if ( bRet == false )	return false;
				break;
			case 2:// Delete
				ulDataType = 0;
				bRet = CheckDataType( pRefItm, &ulDataType );
				if ( bRet == false )
				{
					puts( "Movie object is not supported.\n" );
					break;
				}
				bRet = DeleteDramCapability( pRefItm, ulItmID );
				if ( g_bFileRemoved )
				{
					// If Delete was succeed, Item object must be closed. 
					RemoveChild( pRefSrc, ulItmID );
					pRefItm = NULL;
				}
				break;
			case 3:// IsAlive
				bRet = SetBoolCapability( pRefItm, kNkMAIDCapability_IsAlive );
				break;
			case 4:// Name
				bRet = SetStringCapability( pRefItm, kNkMAIDCapability_Name );
				break;
			case 5:// DataTypes
				bRet = SetUnsignedCapability( pRefItm, kNkMAIDCapability_DataTypes );
				break;
			case 6:// DateTime
				bRet = SetDateTimeCapability( pRefItm, kNkMAIDCapability_DateTime );
				break;
			case 7:// StoredBytes
				bRet = SetUnsignedCapability( pRefItm, kNkMAIDCapability_StoredBytes );
				break;
			default:
				wSel = 0;
		}
		if ( bRet == false ) {
			printf( "An Error occured. Enter '0' to exit.\n>" );
			scanf( "%s", buf );
			bRet = true;
		}
	} while( wSel > 0 && pRefItm != NULL );

	if ( pRefItm != NULL ) {
		// If the item object remains, close it and remove from parent link.
		bRet = RemoveChild( pRefSrc, ulItmID );
	}

	return bRet;
}
//------------------------------------------------------------------------------------------------------------------------------------
//
BOOL ImageCommandLoop( LPRefObj pRefItm, ULONG ulDatID )
{
	LPRefObj	pRefDat = NULL;
	char	buf[256];
	UWORD	wSel;
	BOOL	bRet = true;

	pRefDat = GetRefChildPtr_ID( pRefItm, ulDatID );
	if ( pRefDat == NULL ) {
		// Create Image object and RefSrc structure.
		if ( AddChild( pRefItm, ulDatID ) == true ) {
			printf("Image object is opened.\n");
		} else {
			printf("Image object can't be opened.\n");
			return false;
		}
		pRefDat = GetRefChildPtr_ID( pRefItm, ulDatID );
	}

	// command loop
	do {
		printf( "\nSelect (1-6, 0)\n" );
		printf( " 1. IsAlive                  2. Name                     3. StoredBytes\n" );
		printf( " 4. Pixels                   5. RawJpegImageStatus       6. Acquire\n" );
		printf( " 0. Exit\n>" );
		scanf( "%s", buf );
		wSel = atoi( buf );

		switch( wSel )
		{
			case 1:// IsAlive
				bRet = SetBoolCapability( pRefDat, kNkMAIDCapability_IsAlive );
				break;
			case 2:// Name
				bRet = SetStringCapability( pRefDat, kNkMAIDCapability_Name );
				break;
			case 3:// StoredBytes
				bRet = SetUnsignedCapability( pRefDat, kNkMAIDCapability_StoredBytes );
				break;
			case 4:// Show Pixels
				// Get to know how many pixels there are in this image.
				bRet = SetSizeCapability( pRefDat, kNkMAIDCapability_Pixels );
				break;
			case 5:// RawJpegImageStatus
				bRet = SetUnsignedCapability( pRefDat, kNkMAIDCapability_RawJpegImageStatus );
				break;
			case 6:// Acquire
				bRet = IssueAcquire( pRefDat );
				// The item has been possibly removed.
				break;
			default:
				wSel = 0;
		}
		if ( bRet == false ) {
			printf( "An Error occured. Enter '0' to exit.\n>" );
			scanf( "%s", buf );
			bRet = true;
		}
	} while( wSel > 0 && g_bFileRemoved == false );

// Close Image_Object
	bRet = RemoveChild( pRefItm, ulDatID );

	return bRet;
}
//------------------------------------------------------------------------------------------------------------------------------------
//
BOOL MovieCommandLoop( LPRefObj pRefItm, ULONG ulDatID )
{
	LPRefObj	pRefDat = NULL;
	char	buf[256];
	UWORD	wSel;
	BOOL	bRet = true;

	pRefDat = GetRefChildPtr_ID( pRefItm, ulDatID );
	if ( pRefDat == NULL ) {
		// Create Movie object and RefSrc structure.
		if ( AddChild( pRefItm, ulDatID ) == true ) {
			printf("Movie object is opened.\n");
		} else {
			printf("Movie object can't be opened.\n");
			return false;
		}
		pRefDat = GetRefChildPtr_ID( pRefItm, ulDatID );
	}

	// command loop
	do {
		printf( "\nSelect (1-5, 0)\n" );
		printf( " 1. IsAlive                  2. Name                     3. StoredBytes\n" );
		printf( " 4. Pixels                   5. GetVideoImage\n" );
		printf( " 0. Exit\n>" );
		scanf( "%s", buf );
		wSel = atoi( buf );

		switch( wSel )
		{
			case 1:// IsAlive
				bRet = SetBoolCapability( pRefDat, kNkMAIDCapability_IsAlive );
				break;
			case 2:// Name
				bRet = SetStringCapability( pRefDat, kNkMAIDCapability_Name );
				break;
			case 3:// StoredBytes
				bRet = SetUnsignedCapability( pRefDat, kNkMAIDCapability_StoredBytes );
				break;
			case 4:// Show Pixels
				// Get to know how many pixels there are in this image.
				bRet = SetSizeCapability( pRefDat, kNkMAIDCapability_Pixels );
				break;
			case 5:// GetVideoImage
				bRet = GetVideoImageCapability( pRefDat, kNkMAIDCapability_GetVideoImage );
				// The item has been possibly removed.
				break;
			default:
				wSel = 0;
		}
		if ( bRet == false ) {
			printf( "An Error occured. Enter '0' to exit.\n>" );
			scanf( "%s", buf );
			bRet = true;
		}
	} while( wSel > 0 && g_bFileRemoved == false );

// Close Movie_Object
	bRet = RemoveChild( pRefItm, ulDatID );

	return bRet;
}
//------------------------------------------------------------------------------------------------------------------------------------
//
BOOL ThumbnailCommandLoop( LPRefObj pRefItm, ULONG ulDatID )
{
	LPRefObj	pRefDat = NULL;
	char	buf[256];
	UWORD	wSel;
	BOOL	bRet = true;

	pRefDat = GetRefChildPtr_ID( pRefItm, ulDatID );
	if ( pRefDat == NULL ) {
		// Create Thumbnail object and RefSrc structure.
		if ( AddChild( pRefItm, ulDatID ) == true ) {
			printf("Thumbnail object is opened.\n");
		} else {
			printf("Thumbnail object can't be opened.\n");
			return false;
		}
		pRefDat = GetRefChildPtr_ID( pRefItm, ulDatID );
	}

	// command loop
	do {
		printf( "\nSelect (1-5, 0)\n" );
		printf( " 1. IsAlive                  2. Name                     3. StoredBytes\n" );
		printf( " 4. Pixels                   5. Acquire \n" );
		printf( " 0. Exit\n>" );
		scanf( "%s", buf );
		wSel = atoi( buf );

		switch( wSel )
		{
			case 1:// IsAlive
				bRet = SetBoolCapability( pRefDat, kNkMAIDCapability_IsAlive );
				break;
			case 2:// Name
				bRet = SetStringCapability( pRefDat, kNkMAIDCapability_Name );
				break;
			case 3:// StoredBytes
				bRet = SetUnsignedCapability( pRefDat, kNkMAIDCapability_StoredBytes );
				break;
			case 4:// Pixels
				// Get to know how many pixels there are in this image.
				bRet = SetSizeCapability( pRefDat, kNkMAIDCapability_Pixels );
				break;
			case 5:// Acquire
				bRet = IssueAcquire( pRefDat );
				break;
			default:
				wSel = 0;
		}
		if ( bRet == false ) {
			printf( "An Error occured. Enter '0' to exit.\n>" );
			scanf( "%s", buf );
			bRet = true;
		}
	} while( wSel > 0 );

// Close Thumbnail_Object
	bRet = RemoveChild( pRefItm, ulDatID );

	return bRet;
}

//------------------------------------------------------------------------------------------------------------------------------------
//
