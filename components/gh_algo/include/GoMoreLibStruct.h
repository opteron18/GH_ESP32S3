#ifndef __GoMoreAlgoHalStruct__
#define __GoMoreAlgoHalStruct__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ***********  updateIndex  ************ */
// UpdateFitnessOutput
union UpdateWorkoutOutput {
    struct UpdateSwimOutput{
        int startIndex;
        int endIndex;
        uint16_t cntStroke;
        uint8_t type;
        uint8_t frontcrawlPerc;
        uint8_t breaststrokePerc;
        uint8_t backstrokePerc;
        uint8_t butterflyPerc;
        uint8_t status;
    } swim;
    // Run Hand
    struct UpdateRunOutput{
        // run stance, old module output
        float orgCadence;
        float orgStance;
        float orgStaBalance;

        // run form
        float cadence;
        float stance;
        float staBalance;
        float verticalOscillation;
        float runPowerWatt;
        float stepLen;

        // run summary
        int totalStep;
        float pace;
        float maxPace;
        float minPace;
        float avgPace;
        float avgSpeed;
        float avgCadence;
    } run;
    struct UpdateJumpRopeOutput{
        int totalCount;
        int tripCount;
        int sessionCount;
    } jumpRope;
    struct UpdateSkiingOutput{
        float avgSpeed;
        float maxSpeed;
        float distance;
    } skiing;
    struct UpdateSoccerOutput{
        float avgSpeed;
        float maxSpeed;
        float distance;
    } soccer;
    struct UpdateBaseballOutput{
        int count;
    } baseball;
    struct UpdateGolfOutput{
        int backswing;
        int downswing;
        float tempo;
        int count;
    } golf;
    struct UpdateRowingOutput{
        float cadence;
        uint32_t rowCnt;
    } row;
    struct UpdateTabletennisOutput{
        uint32_t cntFore;
        uint32_t cntBack;
        uint32_t cntTotal;
    } tabletennis;
    // Run Foot
    struct UpdateRunFormFootOutput{
        int stepCntTotal;
        float cadence;
        float stance;
        float flight;
        float stanceFlightRatio;
        float speed;                // kph
        float distance;
        float stepLen;
        float cadencePaceRatio;
        float pace;                 // unit: minute per km
        
        float groundImpact;
        float footStrikeForefootPerc;
        float footStrikeMidfootPerc;
        float footStrikeHeelPerc;
        uint8_t footStrikeMajor; // 0/1/2:forefoot/midfoot/heel
        float eversionDegree;

        float staBalance;
        float runPowerWatt;
    } runFormFoot;
    struct UpdateCyclingFormOutput{
        float cadence;
    } cyclingForm;
    struct UpdateEllipticalOutput{
        float cadence;
        int steps;
    } elliptical;
    struct UpdateDumbbellOutput{
        int dumbbellCount;
    } dumbbell;
    struct UpdateBoxingOutput{
        uint16_t punchCount;
    } boxing;
    struct UpdateStaticBalanceOutput{
        uint8_t level;
        uint8_t status;
    } staticBalance;
    struct UpdateBodyweightTrainingOutput{
        uint32_t count;
    } bodyweightTraining;
    struct UpdateBasketballOutput{
        int shotCounts;
        float dribbleTime;
        uint8_t dribbleSectionStatus;
    } basketball;
    struct UpdateFrisbeeOutput{
        uint32_t count;
    } frisbee;
    struct UpdateCricketOutput{
        int count;
    } cricket;
    struct UpdateSquashOutput{
        int cntFore;
        int cntBack;
		int cntSwing;
    } squash;
    struct UpdateTennisOutput{
        int cntFore;
        int cntBack;
		int cntSwing;
    } tennis;
    struct UpdateBadmintonOutput{
        int cntFore;
        int cntBack;
		int cntSwing;
    } badminton;
};

typedef struct mFitnessUpdate{
    float aerobic;
    float anaerobic;
    float stamina;
    float teAerobic;
    float teAnaerobic;
    float teStamina;
    float hrTeA;
    float hrTeAn;
    int filterHr;
    int hrZone;
} mFitnessUpdate;

struct UpdateFitnessOutput {
    union UpdateWorkoutOutput workout;
    struct mFitnessUpdate fitness;
};

typedef struct IndexIO {
    // Input
    uint32_t timestamp;
    // int16_t timeZoneOffset;
    // float* ppg1;
    // float* ppg2;
    // float* ppg3;
    // uint16_t* ppiRef;
    float* accX;
    float* accY;
    float* accZ;
    // float* gyroX;
    // float* gyroY;
    // float* gyroZ;
    // float* ecg;
    float* gsr;
    // uint8_t ppgNumChannels;
    // uint8_t ppgLength;
    // uint8_t ppiSize;
    uint8_t accLength;
    // uint8_t gyroLength;
    // uint8_t ecgLength;
    uint8_t gsrLength;
    // float longitude;
    // float latitude;
    // float altitude;
    // float gpsSpeed;
    // float gpsAccuracy;
    // float incline;
    // float hrRef;
    // float speedRef;
    // float cyclingCadence;
    // float cyclingPower;
    // float cyclingResistance;
    // float bodyTemperature;
    // int8_t sleepStage;
    // int8_t sleepStageStatus;
    // float spo2;
    float* biaI;
    float* biaQ;
    int8_t* biaFlag;
    uint8_t biaLength;

    // Output
    // float hrOut;
    // float paceOut;
    // float speedOut;
    // float stepCountOut;
    // float stressOut;
    // float allDayStaminaOut;
    // float vo2maxOut;
    // float kcalOut;
    // float METOut;
    // float fatOut;
    // float carbOut;
    // float METsOut[5];
    // uint8_t activityScoreOut; // range 0 ~ 100
    // uint8_t activityScoreStatusOut;
    // float rrOut;
    // float rrConfOut;
    // float hrRestOut;
    // uint8_t hrRestEstStatusOut;
    // float hrvOut;
    // float actCountOut;
    // float speedGpsOut;
    // float baseHrvOut;
    // float baseHrOut;
    // int8_t speedStatusOut;
    // int8_t speedSourceOut;
    // int8_t activeTypeOut;
    // uint8_t cadenceOut;
    // uint8_t pqiFreqOut;
    // uint8_t pqiTimeOut;
    // uint8_t pqiStatusOut;
    // uint8_t ppgHrFlagOut;
    //
    // uint8_t accStageOut;
    //
    // uint8_t sleepPeakOut[16];
    // uint8_t sleepPeakSizeOut;
    // uint8_t sleepPeakStatusOut;
    //
    // int8_t sleepStageOut;
    // int8_t sleepStageStatusOut;
    // int8_t sleepMovementLevelOut;       // 0 ~ 3: No / Low / Medium / Intense Movement
    // int8_t sleepMovementStatusOut;      // 0: No Output, 1: Output Level
    //
    // uint32_t sleepPeriodStartTSOut;
    // uint32_t sleepPeriodEndTSOut;
    // uint16_t sleepPeriodNumMinutesOut;
    // uint8_t sleepPeriodStatusOut;
    // uint8_t sleepFeatOut[40];
    // uint8_t sleepFeatSizeOut;
    // uint32_t sleepFeatOffsetOut;
    //
    // // Fitness Output
    //
	// //Fitness machine cali out
	// int16_t calibStatusOut;
    //
    // // int avgHrOut;
    // // float avgSpdOut;
    // // int avgCadenceOut;
    // // int totalStepOut;
    // // int durationOut;
    //
    // // Fall Detect Output
    // uint8_t fallDetectOut;
    //
    //
    // // Mixed Exercise Training
    // uint8_t metStatusOut;
    // uint8_t metRepsOut[40];
    // float   metKcalOut;
    // uint8_t metRemainingStepSecsOut;
    // uint8_t metCompletionRateOut;
    // uint8_t metEffortsOut[40];
    // uint16_t metTrainingLoadOut[4]; // cardio, upper, lower, core
    // uint8_t metPerformanceOut[2]; // cardio, muscular
    // uint8_t metAasOut;
    // uint8_t metChecksumOut[23+6];
    //
    // // Chronotype
    // // float chronotypeFeatOut;
    // // uint8_t chronotypeFeatStatusOut;
    // uint8_t chronotypeOut;
    // uint8_t chronotypeConfOut;
    // uint8_t chronotypeIdealbedOut;
    // uint8_t chronotypeStatusOut;
    //
    // // Ppi from PPG-HR
    // uint16_t ppiPpgOut[40];
    // uint8_t ppiPpgSizeOut;
    //
    // // Ppi Td
    // uint16_t ppiTdOut[120];
    // uint8_t ppiTdBeatNumOut;
    // uint8_t ppiTdStatusOut;
    //
    // // Ppg Afib
    // uint8_t ppgAfibClassOut;
    // uint8_t ppgAfibConfOut;
    // uint8_t ppgAfibStatusOut;
    //
    // // Ecg Afib
    // uint8_t ecgAfibClassOut;
    // uint8_t ecgAfibConfOut;
    // uint8_t ecgAfibStatusOut;
    //
    // // RRI
    // uint16_t rriOut[120];
    // uint8_t rriBeatNumOut;
    // uint8_t rriStatusOut;
    //
    // // Wrist Off
    // uint8_t wristOffOut;
    //
    // // Fitness
    // struct UpdateFitnessOutput fitnessOut;
    //
    // // workout auto detection (15 min+)
    // uint8_t workoutSummaryStatusOut; // 1: output workout summary
    // uint32_t workoutStartTSOut;
    // uint32_t workoutDurationOut; // seconds
    // uint32_t workoutStepOut;
    // uint8_t workoutIntensityOut; // 0: Low, 1: Medium, 2: High
    // uint8_t workoutTypeOut; // 0: undefined, 1: walk, 2: run
    // float workoutKcalOut;
    //
    // // OSA
    // uint8_t osaOut;
    // uint8_t osaSpo2ConfOut;
    // uint8_t osaStatusOut;

    // GSR
    uint8_t gsrStressOut;
    uint8_t gsrEmotionOut;
	uint8_t gsrConfOut;
    uint8_t gsrStatusOut;

} IndexIO;

/* ***********  startFitness  ************ */
// StartFitnessInput
union StartWorkoutInput{
    struct StartSwimInput {
        uint8_t handFlag;
        uint8_t poolSize;                   // unit: meter
    } swim;
    // Run Hand
    struct StartRunInput {
        uint8_t handFlag;
        uint8_t gender;
        float weight;
        float height;
    } run;
    struct StartSoccerInput {
        uint8_t signalSource;               // 0: Gps Only, 1: Gps + Acc
    } soccer;
    struct StartRunFormFootInput {
    // Run Foot
        uint8_t footRLFlag;
        float weight;
    } runFormFoot;
    struct StartBodyweightTrainingInput {
        uint8_t movement;
    } bodyweightTraining;
    struct StartSquashInput {
        uint8_t handFlag;
    } squash;
    struct StartTennisInput {
        uint8_t handFlag;
    } tennis;
    struct StartBadmintonInput {
        uint8_t handFlag;
    } badminton;
    struct StartTabletennisInput {
        uint8_t racketType;
    } tabletennis;
};

typedef struct mFitnessStart {
    uint32_t rtc;
    char *trainingInfo;
} mFitnessStart;

struct StartFitnessInput {
    int32_t workoutType;
    union StartWorkoutInput workout;
    struct mFitnessStart fitness;
};

/* ***********  endFitness  ************ */
// EndFitnessInput
union EndWorkoutInput {
    uint8_t tmp;
};

typedef struct mFitnessInfo {
    uint32_t workoutDate[1];
} mFitnessInfo;

struct EndFitnessInput {
    union EndWorkoutInput workout;
    struct mFitnessInfo fitness;
};

// EndFitnessOutput
union EndWorkoutOutput{
    struct EndSwimOutput{
        int startIndex;
        int endIndex;
        uint16_t cntStroke;
        uint8_t type;
        uint8_t frontcrawlPerc;
        uint8_t breaststrokePerc;
        uint8_t backstrokePerc;
        uint8_t butterflyPerc;
        uint8_t status;

        // Summary
        uint16_t strokeCnt;
        float strokePerMin;
        float strokePerLap;
        float swolf;
        uint8_t cntLap;
        uint32_t swimDur;
    } swim;
    struct EndCmjPredictOutput{
        float slj;
        float sprint50;
        uint8_t status;
    } cmjPredict;
    // Run Foot
    struct EndRunFormFootOutput{
        float meanCadence;
        float maxCadence;
        float meanStepLen;
        float meanGroundImpact;
        float maxGroundImpact;
        float meanStance;
        float minStance;
        float meanFlight;
        float maxFlight;
        float meanStanceFlightRatio;
        float minStanceFlightRatio;
        float meanPace;              // unit: minute per km
        float maxPace;               // unit: minute per km
        uint8_t landingFormMajor;  // 1/2/3:toes/sole/heel
    } runFormFoot;
    struct EndCyclingFormOutput {
        float meanCadence;
        float maxCadence;
    } cyclingForm;
};
typedef struct mFitnessSummary {
    float runLevelOut[9];
    float vValueOut[2];
    float pace[2];
    float cyclingLevelOut[9];
    float cValueOut[2];
    float cyclingIndex[2];  /* [0]:ftp [1]:powerPeak */
    float efficiency[2];
    int recoveryTime[1];
    int bestRunTime[11];
    uint16_t trainingLoad[1];
    float tlTrend[1];
    int8_t trainingStatus[1];
    int8_t fitnessAge[2];
    float hrEstorOut[1];
    char trainChecksum[128];
    uint16_t trainChecksumLen[1];
    float pMAP[8];
    int   dMAP[8];
} mFitnessSummary;

struct EndFitnessOutput {
    union EndWorkoutOutput workout;
    struct mFitnessSummary fitness;
};

/* ***********  startHealth  ************ */
typedef union StartActionInput {
    uint8_t padding; // No functional purpose, it is only included to prevent empty structures or unions.
    struct StartBodyCompositionInput {
        float weight;
        float height;
    } bodyComposition;
} StartInput;

typedef struct StartHealthInput {
    int32_t actionType;
    union StartActionInput action;
} StartHealthInput;

/* ***********  endHealth  ************ */
typedef union EndActionInput {
    uint8_t padding; // No functional purpose, it is only included to prevent empty structures or unions.
    struct EndBodyCompositionInput {
        int success;
    } bodyComposition;
} EndInput;

typedef struct EndHealthInput {
    uint8_t padding; // No functional purpose, it is only included to prevent empty structures or unions.
    union EndActionInput action;
} EndHealthInput;

typedef union EndActionOutput {
    uint8_t padding; // No functional purpose, it is only included to prevent empty structures or unions.
    struct EndBodyCompositionOutput{
        float bmr;          // unit: kcal
        float water;        // unit: l
        float protein;      // unit: kg
        float minerals;     // unit: kg
        float fat;          // unit: kg
        float smm;          // unit: kg
        float ffm;          // unit: kg
        uint8_t status;     // 0: success, 1: fail
    } bodyComposition;
} EndOutput;

typedef struct EndHealthOutput {
    uint8_t padding; // No functional purpose, it is only included to prevent empty structures or unions.
    union EndActionOutput action;
} EndHealthOutput;

/* ***********  setAuthParameters  ************ */
typedef struct workoutAuthParameters { 
    const char *pKey; 
    /* Base64 decoded pkey. This key gets from 
       GoMore system by calling API or provided by 
       GoMore administrator. */ 
    const char *deviceId; 
    /* Device ID registered in GoMore system. */ 
    uint8_t devIdLen; 
    /* Length of device ID. */ 
    uint32_t rtcCurrentTime;
    /* RTC time of the device. (Unix timestamp) */
} workoutAuthParameters;

/* ***********  chronotype  ************ */
typedef struct ChronotypeOutput {
    uint8_t chronotypeOut;
    uint8_t chronotypeConfOut;
    uint8_t chronotypeIdealbedOut;
    uint8_t chronotypeStatusOut;
} ChronotypeOutput;

/* ***********  setSleepConfig  ************ */
typedef struct SleepConfig {
    int16_t minNapLength;               // unit: minute, range: [15, -], default: 20
    int16_t maxCombineLength;           // unit: minute, range: [0, 120], default: 60
    uint8_t disablePeriodFinetuning;    // default: 0 (disable period finetuning or not)
    uint8_t disableAutoDetection;       // default: 0 (disable auto detection or not)

} SleepConfig;

/* ***********  startSleepPeriod, endSleepPeriod  ************ */
typedef struct SleepOutput {
    uint32_t sleepPeriodStartTSOut;
    uint32_t sleepPeriodEndTSOut;
    uint16_t sleepPeriodNumMinutesOut;
    uint8_t sleepPeriodStatusOut;
    uint8_t sleepFeatOut[40];
    uint8_t sleepFeatSizeOut;
    uint32_t sleepFeatOffsetOut;
    
    struct ChronotypeOutput chronoOut;
} SleepOutput;


/* ***********  getSleepSummary ************ */
typedef struct SleepSummaryInput {
    int8_t mode;       // 0: get sleep index from the latest valid sleep period / 1: get sleep Index with manual timestamps
    uint32_t startTS;
    uint32_t endTS;
} SleepSummaryInput;

typedef struct SleepSummaryOutput {
    uint32_t startTS;
    uint32_t endTS;
    int8_t *stages;
    int8_t *movements;
    int32_t numEpochs;      // Number of Stages
    int32_t numMovements;   // Number of Movements == floor(Number of Stages / 2)
    float totalSleepTime;
    float waso;
    float sleepPeriod;
    float efficiency;
    float wakeRatio;
    float remRatio;
    float lightRatio;
    float deepRatio;
    float wakeNumMinutes;
    float remNumMinutes;
    float lightNumMinutes;
    float deepNumMinutes;
    float score;
    uint8_t type;
} SleepSummaryOutput;


/* ***********  setMixedExerSession  ************ */
typedef struct SessionInfo {
    uint8_t numSteps;
    uint8_t numSets;
    uint8_t* steps;
    uint8_t* duration;
    uint8_t* effort;
    uint8_t* circuitStartEnd;
    uint8_t* checksum;
} SessionInfo;

/* ***********  updateGesture ************ */
typedef struct IndexIOGesture {
    uint64_t timestamp;     // ms
    float* accX;
    float* accY;
    float* accZ;
    float* gyroX;
    float* gyroY;
    float* gyroZ;
    uint8_t accLength;
    uint8_t gyroLength;
    uint8_t gestureTypeOut;
    uint8_t gestureStatusOut;   // 0: No Event, 1: Gesture Detected.
    
} IndexIOGesture;

/* ***********  analyzeWomenHealthCycle ************ */
#define WOMEN_HEALTH_POTENTIAL_CYCLE 100
#define WOMEN_HEALTH_CYCLES_TO_PREDICT 5
typedef struct ModuleWomenHealthInput{
    int8_t lastMensperiod;
    int8_t cycleDuration;
}ModuleWomenHealthInput;

typedef struct ModuleWomenHealthOutput{
    int8_t potentialFo[WOMEN_HEALTH_POTENTIAL_CYCLE];
    int8_t potentialLu[WOMEN_HEALTH_POTENTIAL_CYCLE];
    int8_t potentialOv[WOMEN_HEALTH_POTENTIAL_CYCLE];
    int8_t potentialMens[WOMEN_HEALTH_POTENTIAL_CYCLE];
    int ovuDistance;
    int mensDistance;
    int nextOvu[WOMEN_HEALTH_CYCLES_TO_PREDICT];
    int nextMens[WOMEN_HEALTH_CYCLES_TO_PREDICT];
}ModuleWomenHealthOutput;

#ifdef __cplusplus
        }
#endif
#endif /* __GoMoreAlgoHalStruct__ */
