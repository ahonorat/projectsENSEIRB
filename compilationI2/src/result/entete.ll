target triple = "x86_64-unknown-linux-gnu"

%struct.CarElt = type {
	i32,
	%struct.tInitCar,
	%struct.tPublicCar,
	%struct.tCarRaceInfo,
	%struct.tPrivCar,
	%struct.tCarCtrl,
	%struct.tCarPitCmd,
	%struct.RobotItf*,
	%struct.CarElt*
}

%struct.tInitCar = type {
	[32 x i8],
	[32 x i8],
	[32 x i8],
	[32 x i8],
	i32,
	i32,
	i32,
	i32,
	[3 x float],
	%struct.t3Dd,
	%struct.t3Dd,
	%struct.t3Dd,
	float,
	float,
	%struct.t3Dd,
	[4 x %struct.tWheelSpec],
	%struct.tVisualAttributes
}

%struct.t3Dd = type {
	float,
	float,
	float
}

%struct.tWheelSpec = type {
	float,
	float,
	float,
	float,
	float
}

%struct.tVisualAttributes = type {
	i32,
	[2 x %struct.t3Dd],
	float
}

%struct.tPublicCar = type {
	%struct.tDynPt,
	%struct.tDynPt,
	[4 x [4 x float]],
	%struct.tTrkLocPos,
	i32,
	[4 x %struct.tPosd]
}

%struct.tDynPt = type {
	%struct.tPosd,
	%struct.tPosd,
	%struct.tPosd
}

%struct.tPosd = type {
	float,
	float,
	float,
	float,
	float,
	float
}

%struct.tTrkLocPos = type {
	%struct.trackSeg*,
	i32,
	float,
	float,
	float,
	float
}

%struct.trackSeg = type {
	i8*,
	i32,
	i32,
	i32,
	i32,
	float,
	float,
	float,
	float,
	float,
	float,
	float,
	float,
	float,
	%struct.t3Dd,
	[4 x %struct.t3Dd],
	[7 x float],
	float,
	float,
	float,
	%struct.t3Dd,
	i32,
	float,
	i32,
	float,
	%struct.SegExt*,
	%struct.trackSurface*,
	[2 x %struct.trackBarrier*],
	%struct.RoadCam*,
	%struct.trackSeg*,
	%struct.trackSeg*,
	%union.anon.0
}

%struct.SegExt = type {
	i32,
	i32*
}

%struct.trackSurface = type {
	%struct.trackSurface*,
	i8*,
	float,
	float,
	float,
	float,
	float,
	float
}

%struct.trackBarrier = type {
	i32,
	float,
	float,
	%struct.trackSurface*,
	%class.v2t
}

%class.v2t = type {
	%union.anon
}

%union.anon = type {
	%struct.anon
}

%struct.anon = type {
	float,
	float
}

%struct.RoadCam = type {
	i8*,
	%struct.t3Dd,
	%struct.RoadCam*
}

%union.anon.0 = type {
	%struct.anon.1
}

%struct.anon.1 = type {
	%struct.trackSeg*,
	%struct.trackSeg*
}

%struct.tCarRaceInfo = type {
	double,
	i8,
	double,
	double,
	double,
	double,
	float,
	float,
	i32,
	i32,
	i32,
	i32,
	double,
	i32,
	double,
	double,
	float,
	float,
	double,
	%struct.TrackOwnPit*,
	i32,
	%struct.CarPenaltyHead
}

%struct.TrackOwnPit = type {
	%struct.tTrkLocPos,
	i32,
	float,
	float,
	i32,
	[4 x %struct.CarElt*]
}

%struct.CarPenaltyHead = type {
	%struct.CarPenalty*,
	%struct.CarPenalty**
}

%struct.CarPenalty = type {
	i32,
	i32,
	%struct.anon.2
}

%struct.anon.2 = type {
	%struct.CarPenalty*,
	%struct.CarPenalty**
}

%struct.tPrivCar = type {
	i8*,
	i8*,
	i32,
	[32 x i8],
	[4 x %struct.tWheelState],
	[4 x %struct.tPosd],
	i32,
	float,
	float,
	float,
	float,
	float,
	float,
	float,
	float,
	[10 x float],
	i32,
	i32,
	[4 x float],
	[4 x float],
	i32,
	i32,
	float,
	%struct.t3Dd,
	%struct.t3Dd,
	i32,
	i32,
	%struct.tCollisionState_
}

%struct.tWheelState = type {
	%struct.tPosd,
	float,
	float,
	i32,
	%struct.trackSeg*,
	float,
	float,
	float,
	float,
	float,
	float,
	float,
	float,
	float,
	float
}

%struct.tCollisionState_ = type {
	i32,
	[3 x float],
	[3 x float]
}

%struct.tCarCtrl = type {
	float,
	float,
	float,
	float,
	i32,
	i32,
	[4 x [32 x i8]],
	[4 x float],
	i32
}

%struct.tCarPitCmd = type {
	float,
	i32,
	i32,
	%struct.tCarPitSetup
}

%struct.tCarPitSetup = type {
	%struct.tCarPitSetupValue,
	[4 x %struct.tCarPitSetupValue],
	[4 x %struct.tCarPitSetupValue],
	[4 x %struct.tCarPitSetupValue],
	%struct.tCarPitSetupValue,
	%struct.tCarPitSetupValue,
	[4 x %struct.tCarPitSetupValue],
	[4 x %struct.tCarPitSetupValue],
	[4 x %struct.tCarPitSetupValue],
	[4 x %struct.tCarPitSetupValue],
	[4 x %struct.tCarPitSetupValue],
	[4 x %struct.tCarPitSetupValue],
	[2 x %struct.tCarPitSetupValue],
	[2 x %struct.tCarPitSetupValue],
	[2 x %struct.tCarPitSetupValue],
	[2 x %struct.tCarPitSetupValue],
	[2 x %struct.tCarPitSetupValue],
	[8 x %struct.tCarPitSetupValue],
	[2 x %struct.tCarPitSetupValue],
	[3 x %struct.tCarPitSetupValue],
	[3 x %struct.tCarPitSetupValue],
	[3 x %struct.tCarPitSetupValue],
	[3 x %struct.tCarPitSetupValue],
	[3 x %struct.tCarPitSetupValue],
	[3 x %struct.tCarPitSetupValue],
	[3 x i32]
}

%struct.tCarPitSetupValue = type {
	float,
	float,
	float
}

%struct.RobotItf = type {
	void (i32, %struct.tTrack*, i8*, i8**, %struct.Situation*)*,
	 {}*,
	 {}*,
	 {}*,
	 i32 (i32, %struct.CarElt*, %struct.Situation*)*,
	 void (i32)*,
	 i32
}

%struct.tTrack = type {
	i8*,
	i8*,
	i8*,
	i8*,
	i8*,
	i8*,
	i32,
	i32,
	float,
	float,
	%struct.tTrackPitInfo,
	%struct.trackSeg*,
	%struct.trackSurface*,
	%struct.t3Dd,
	%struct.t3Dd,
	%struct.tTrackGraphicInfo
}

%struct.tTrackPitInfo = type {
	i32,
	i32,
	i32,
	i32,
	float,
	float,
	float,
	%struct.trackSeg*,
	%struct.trackSeg*,
	%struct.trackSeg*,
	%struct.trackSeg*,
	%struct.TrackOwnPit*,
	i32,
	i32
}

%struct.tTrackGraphicInfo = type {
	i8*,
	i8*,
	i32,
	[3 x float],
	i32,
	i8**,
	%struct.tTurnMarksInfo
}

%struct.tTurnMarksInfo = type {
	float,
	float,
	float,
	float
}

%struct.Situation = type {
	%struct.tRaceAdmInfo,
	double,
	double,
	i32,
	%struct.CarElt**
}

%struct.tRaceAdmInfo = type {
	i32,
	i32,
	i32,
	i32,
	i32,
	i64
}


