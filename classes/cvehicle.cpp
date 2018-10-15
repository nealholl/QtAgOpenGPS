#include <QtGlobal>
#include <QFile>
#include "cvehicle.h"
#include <QOpenGLContext>
#include <QOpenGLFunctions_2_1>
#include <QOpenGLFunctions>
#include <QRgb>
#include "aogsettings.h"
#include "csection.h"
#include "glm.h"
#include "glutils.h"

//TODO: move these to a central location
#define DEFAULT_TOOLWIDTH 16.0
#define DEFAULT_TOOLOVERLAP 0.1
#define DEFAULT_TOOLTRAILINGHITCHLENGTH -2.0
#define DEFAULT_TANKTRAILINGHITCHLENGTH -1.5
#define DEFAULT_TOOLOFFSET 0.0
#define DEFAULT_ISTOOLBEHINDPIVOT true
#define DEFAULT_ISTOOLTRAILING true
#define DEFAULT_MINAPPLIED 30
#define DEFAULT_TURNOFFDELAY 1.0
#define DEFAULT_NUMSECTIONS 3

#define DEFAULT_ISPIVOTBEHINDANTENNA true
#define DEFAULT_ANTENNAHEIGHT 3.0
#define DEFAULT_ANTENNAPIVOT 1.1

#define DEFAULT_HITCHLENGTH -0.5
#define DEFAULT_WHEELBASE 5.0
#define DEFAULT_ISSTEERINGAXLEAHEAD true
#define DEFAULT_LOOKAHEAD 2.0
#define DEFAULT_SLOWSPEEDCUTOFF 2.0
#define DEFAULT_GOALPOINTLOOKAHEAD 3.0
#define DEFAULT_MAXANGULARVELOCITY 1.0
#define DEFAULT_MAXSTEERINGANGLE 20.0

//TODO: Change to use a default vehicle file
CVehicle::CVehicle()
{
    AOGSettings s;
    //from settings grab the vehicle specifics
    toolWidth = s.value("vehicle/toolWidth", DEFAULT_TOOLWIDTH).toDouble();
    toolOverlap = s.value("vehicle/toolOverlap", DEFAULT_TOOLOVERLAP).toDouble();
    toolTrailingHitchLength = s.value("vehicle/toolTrailingHitchLength",
                                      DEFAULT_TOOLTRAILINGHITCHLENGTH).toDouble();
    tankTrailingHitchLength = s.value("vehicle/tankTrailingHitchLength",
                                      DEFAULT_TANKTRAILINGHITCHLENGTH).toDouble();
    toolOffset = s.value("vehicle/toolOffset",DEFAULT_TOOLOFFSET).toDouble();

    isToolBehindPivot = s.value("vehicle/isToolBehindPivot",
                                DEFAULT_ISTOOLBEHINDPIVOT).toBool();
    isToolTrailing = s.value("vehicle/isToolTrailing",
                             DEFAULT_ISTOOLTRAILING).toBool();

    isPivotBehindAntenna = s.value("vehicle/isPivotBehindAntenna",
                                   DEFAULT_ISPIVOTBEHINDANTENNA).toBool();
    antennaHeight = s.value("vehicle/antennaHeight",DEFAULT_ANTENNAHEIGHT).toDouble();
    antennaPivot = s.value("vehicle/antennaPivot",DEFAULT_ANTENNAPIVOT).toDouble();
    hitchLength = s.value("vehicle/hitchLength",DEFAULT_HITCHLENGTH).toDouble();

    wheelbase = s.value("vehicle/wheelbase",DEFAULT_WHEELBASE).toDouble();
    isSteerAxleAhead = s.value("vehicle/isSteerAxleAhead",
                               DEFAULT_ISSTEERINGAXLEAHEAD).toBool();

    toolLookAhead = s.value("vehicle/lookAhead",DEFAULT_LOOKAHEAD).toDouble();
    toolTurnOffDelay = s.value("vehicle/turnOffDelay",DEFAULT_TURNOFFDELAY).toDouble();

    numOfSections = s.value("vehicle/numSections",DEFAULT_NUMSECTIONS).toInt();
    numSuperSection = numOfSections+1;

    slowSpeedCutoff = s.value("vehicle/slowSpeedCutoff",DEFAULT_SLOWSPEEDCUTOFF).toDouble();
    toolMinUnappliedPixels = s.value("vehicle/minApplied",DEFAULT_MINAPPLIED).toInt();

    goalPointLookAhead = s.value("vehicle/goalPointLookAhead",
                                 DEFAULT_GOALPOINTLOOKAHEAD).toDouble();
    maxAngularVelocity = s.value("vehicle/maxAngularVelocity",
                                 DEFAULT_MAXANGULARVELOCITY).toDouble();
    maxSteerAngle = s.value("vehicle/maxSteerAngle",
                            DEFAULT_MAXSTEERINGANGLE).toDouble();

    //hack to get around tight coupling for now.
    //TODO, implement wrapper methods and pass in the
    //variables the section methods need.
    for (int i=0; i <= MAXSECTIONS; i++) {
        section[i].set_vehicle(this);
    }

    memset(avgSpeed,0,sizeof(double)*10);
}

void CVehicle::SaveVehicleFile(QString file)
{
    QSettings vehicleFile(file, QSettings::IniFormat);
    //QTextStream out(file);

    //writer.WriteLine("Version," + Application.ProductVersion.ToString(CultureInfo.InvariantCulture));
    vehicleFile.setValue("Overlap", toolOverlap);
    vehicleFile.setValue("ToolTrailingHitchLength", toolTrailingHitchLength);
    vehicleFile.setValue("TankTrailingHitchLength", tankTrailingHitchLength);
    vehicleFile.setValue("AntennaHeight", antennaHeight);
    vehicleFile.setValue("LookAhead", toolLookAhead);
    vehicleFile.setValue("AntennaPivot", antennaPivot);

    vehicleFile.setValue("HitchLength", hitchLength);
    vehicleFile.setValue("ToolOffset", toolOffset);
    vehicleFile.setValue("TurnOffDelay", toolTurnOffDelay);
    vehicleFile.setValue("Wheelbase", wheelbase);

    vehicleFile.setValue("IsPivotBehindAntenna", isPivotBehindAntenna);
    vehicleFile.setValue("IsSteerAxleAhead", isSteerAxleAhead);
    vehicleFile.setValue("IsToolBehindPivot", isToolBehindPivot);
    vehicleFile.setValue("IsToolTrailing", isToolTrailing);

    vehicleFile.setValue("Sections", numOfSections);
    vehicleFile.setValue("ToolWidth", toolWidth);

    vehicleFile.setValue("SlowSpeedCutoff", slowSpeedCutoff);
    vehicleFile.setValue("ToolMinUnappliedPixels", toolMinUnappliedPixels);

    vehicleFile.setValue("GoalPointLookAhead", goalPointLookAhead);
    vehicleFile.setValue("MaxSteerAngle", maxSteerAngle);
    vehicleFile.setValue("MaxAngularVelocity", maxAngularVelocity);

    /*
     * Need to find this and set it up properly
   writer.WriteLine("Spinner1," + Properties.Vehicle.Default.setSection_position1.ToString(CultureInfo.InvariantCulture));
   writer.WriteLine("Spinner2," + Properties.Vehicle.Default.setSection_position2.ToString(CultureInfo.InvariantCulture));
   writer.WriteLine("Spinner3," + Properties.Vehicle.Default.setSection_position3.ToString(CultureInfo.InvariantCulture));
   writer.WriteLine("Spinner4," + Properties.Vehicle.Default.setSection_position4.ToString(CultureInfo.InvariantCulture));
   writer.WriteLine("Spinner5," + Properties.Vehicle.Default.setSection_position5.ToString(CultureInfo.InvariantCulture));
   writer.WriteLine("Spinner6," + Properties.Vehicle.Default.setSection_position6.ToString(CultureInfo.InvariantCulture));
   writer.WriteLine("Spinner7," + Properties.Vehicle.Default.setSection_position7.ToString(CultureInfo.InvariantCulture));
   writer.WriteLine("Spinner8," + Properties.Vehicle.Default.setSection_position8.ToString(CultureInfo.InvariantCulture));
   writer.WriteLine("Spinner9," + Properties.Vehicle.Default.setSection_position9.ToString(CultureInfo.InvariantCulture));
   writer.WriteLine("Spinner10," + Properties.Vehicle.Default.setSection_position10.ToString(CultureInfo.InvariantCulture));
   writer.WriteLine("Spinner11," + Properties.Vehicle.Default.setSection_position11.ToString(CultureInfo.InvariantCulture));
   writer.WriteLine("Spinner12," + Properties.Vehicle.Default.setSection_position12.ToString(CultureInfo.InvariantCulture));
   writer.WriteLine("Spinner13," + Properties.Vehicle.Default.setSection_position13.ToString(CultureInfo.InvariantCulture));

   writer.WriteLine("WorkSwitch," + Properties.Settings.Default.setF_IsWorkSwitchEnabled.ToString(CultureInfo.InvariantCulture));
   writer.WriteLine("ActiveLow," + Properties.Settings.Default.setF_IsWorkSwitchActiveLow.ToString(CultureInfo.InvariantCulture));

   writer.WriteLine("CamPitch," + Properties.Settings.Default.setCam_pitch.ToString(CultureInfo.InvariantCulture));

   writer.WriteLine("HeadingFromSource," + Properties.Settings.Default.setGPS_headingFromWhichSource);
   writer.WriteLine("TriangleResolution," + Properties.Settings.Default.setDisplay_triangleResolution.ToString(CultureInfo.InvariantCulture));
   writer.WriteLine("IsMetric," + Properties.Settings.Default.setMenu_isMetric.ToString(CultureInfo.InvariantCulture));
   writer.WriteLine("IsGridOn," + Properties.Settings.Default.setMenu_isGridOn.ToString(CultureInfo.InvariantCulture));
   writer.WriteLine("IsLightBarOn," + Properties.Settings.Default.setMenu_isLightbarOn.ToString(CultureInfo.InvariantCulture));
   writer.WriteLine("IsAreaRight," + Properties.Settings.Default.setMenu_isAreaRight.ToString(CultureInfo.InvariantCulture));
   writer.WriteLine("IsPurePursuitLineOn," + Properties.Settings.Default.setMenu_isPureOn.ToString(CultureInfo.InvariantCulture));
   writer.WriteLine("IsGuideLinesOn," + Properties.Settings.Default.setMenu_isSideGuideLines.ToString(CultureInfo.InvariantCulture));
   writer.WriteLine("AntennaOffset," + Properties.Vehicle.Default.setVehicle_antennaOffset.ToString(CultureInfo.InvariantCulture));
   writer.WriteLine("Empty," + "10");
   writer.WriteLine("Empty," + "10");
   writer.WriteLine("Empty," + "10");

   writer.WriteLine("FieldColorR," + Properties.Settings.Default.setF_FieldColorR.ToString(CultureInfo.InvariantCulture));
   writer.WriteLine("FieldColorG," + Properties.Settings.Default.setF_FieldColorG.ToString(CultureInfo.InvariantCulture));
   writer.WriteLine("FieldColorB," + Properties.Settings.Default.setF_FieldColorB.ToString(CultureInfo.InvariantCulture));
   writer.WriteLine("SectionColorR," + Properties.Settings.Default.setF_SectionColorR.ToString(CultureInfo.InvariantCulture));
   writer.WriteLine("SectionColorG," + Properties.Settings.Default.setF_SectionColorG.ToString(CultureInfo.InvariantCulture));
   writer.WriteLine("SectionColorB," + Properties.Settings.Default.setF_SectionColorB.ToString(CultureInfo.InvariantCulture));

   writer.WriteLine("MinTurningRadius," + Properties.Vehicle.Default.setVehicle_minTurningRadius.ToString(CultureInfo.InvariantCulture));
   writer.WriteLine("YouTurnUseDubins," + Properties.Vehicle.Default.set_youUseDubins.ToString(CultureInfo.InvariantCulture));
   writer.WriteLine("Empty," + "10");
   writer.WriteLine("Empty," + "10");
   writer.WriteLine("Empty," + "10");
   writer.WriteLine("Empty," + "10");

   writer.WriteLine("IMUPitchZero," + Properties.Settings.Default.setIMU_pitchZero.ToString(CultureInfo.InvariantCulture));
   writer.WriteLine("IMURollZero," + Properties.Settings.Default.setIMU_rollZero.ToString(CultureInfo.InvariantCulture));
   writer.WriteLine("IsLogNMEA," + Properties.Settings.Default.setMenu_isLogNMEA.ToString(CultureInfo.InvariantCulture));
   writer.WriteLine("MinFixStep," + Properties.Settings.Default.setF_minFixStep.ToString(CultureInfo.InvariantCulture));

   writer.WriteLine("pidP," + Properties.Settings.Default.setAS_Kp.ToString(CultureInfo.InvariantCulture));
   writer.WriteLine("pidI," + Properties.Settings.Default.setAS_Ki.ToString(CultureInfo.InvariantCulture));
   writer.WriteLine("pidD," + Properties.Settings.Default.setAS_Kd.ToString(CultureInfo.InvariantCulture));
   writer.WriteLine("pidO," + Properties.Settings.Default.setAS_Ko.ToString(CultureInfo.InvariantCulture));
   writer.WriteLine("SteerAngleOffset," + Properties.Settings.Default.setAS_steerAngleOffset.ToString(CultureInfo.InvariantCulture));
   writer.WriteLine("minPWM," + Properties.Settings.Default.setAS_minSteerPWM.ToString(CultureInfo.InvariantCulture));
   writer.WriteLine("MaxIntegral," + Properties.Settings.Default.setAS_maxIntegral.ToString(CultureInfo.InvariantCulture));
   writer.WriteLine("CountsPerDegree," + Properties.Settings.Default.setAS_countsPerDegree.ToString(CultureInfo.InvariantCulture));
   writer.WriteLine("Empty," + "10");
   writer.WriteLine("Empty," + "10");

   writer.WriteLine("SequenceFunctionEnter;" + Properties.Vehicle.Default.seq_FunctionEnter);
   writer.WriteLine("SequenceFunctionExit;" + Properties.Vehicle.Default.seq_FunctionExit);
   writer.WriteLine("SequenceActionEnter;" + Properties.Vehicle.Default.seq_ActionEnter);
   writer.WriteLine("SequenceActionExit;" + Properties.Vehicle.Default.seq_ActionExit);
   writer.WriteLine("SequenceDistanceEnter;" + Properties.Vehicle.Default.seq_DistanceEnter);
   writer.WriteLine("SequenceDistanceExit;" + Properties.Vehicle.Default.seq_DistanceExit);

   writer.WriteLine("FunctionList;" + Properties.Vehicle.Default.seq_FunctionList);
   writer.WriteLine("ActionList;" + Properties.Vehicle.Default.seq_ActionList);

   writer.WriteLine("RollFromBrick," + Properties.Settings.Default.setIMU_isRollFromBrick);
   writer.WriteLine("HeadingFromBrick," + Properties.Settings.Default.setIMU_isHeadingFromBrick);
   writer.WriteLine("RollFromDogs," + Properties.Settings.Default.setIMU_isRollFromDogs);
   writer.WriteLine("HeadingFromBNO," + Properties.Settings.Default.setIMU_isHeadingFromBNO);
   writer.WriteLine("Empty," + "10");
   writer.WriteLine("Empty," + "10");
   writer.WriteLine("Empty," + "10");
   writer.WriteLine("Empty," + "10");
   writer.WriteLine("Empty," + "10");
   writer.WriteLine("Empty," + "10");
 */
}

void CVehicle::LoadVehicleFile(QString file)
{
    QSettings vehicleFile(file, QSettings::IniFormat);
    /*
    TODO: implement the program version and error checking
    if (words[0] != "Version")

    {
        var form = new FormTimedMessage(5000, "Vehicle File is Wrong Version", "Must be Version " + Application.ProductVersion.ToString(CultureInfo.InvariantCulture) + " or higher");
        form.Show();
        return false;
    }
    double test = double.Parse(words[1], CultureInfo.InvariantCulture);
    double ver = Convert.ToDouble(Application.ProductVersion.ToString(CultureInfo.InvariantCulture));
    if (ver > 9) ver *= 0.1;
    {

    }
    if (test < ver)
    {
        var form = new FormTimedMessage(5000, "Vehicle File is Wrong Version", "Must be Version "+ Application.ProductVersion.ToString(CultureInfo.InvariantCulture)+ " or higher");
        form.Show();
        return false;
    }

    */
    toolOverlap = vehicleFile.value("Overlap", DEFAULT_TOOLOVERLAP).toDouble();
    toolTrailingHitchLength = vehicleFile.value("ToolTrailingHitchLength", DEFAULT_TOOLTRAILINGHITCHLENGTH).toDouble();
    tankTrailingHitchLength = vehicleFile.value("TankTrailingHitchLength", DEFAULT_TANKTRAILINGHITCHLENGTH).toDouble();
    antennaHeight = vehicleFile.value("AntennaHeight", DEFAULT_ANTENNAHEIGHT ).toDouble();
    toolLookAhead = vehicleFile.value("LookAhead", DEFAULT_LOOKAHEAD).toDouble();
    antennaPivot = vehicleFile.value("AntennaPivot", DEFAULT_ANTENNAPIVOT).toDouble();

    hitchLength = vehicleFile.value("HitchLength", DEFAULT_HITCHLENGTH).toDouble();
    toolOffset = vehicleFile.value("ToolOffset", DEFAULT_TOOLOFFSET).toDouble();
    toolTurnOffDelay = vehicleFile.value("TurnOffDelay", DEFAULT_TURNOFFDELAY).toDouble();
    wheelbase = vehicleFile.value("Wheelbase", DEFAULT_WHEELBASE).toDouble();

    isPivotBehindAntenna = vehicleFile.value("IsPivotBehindAntenna", DEFAULT_ISPIVOTBEHINDANTENNA).toBool();
    isSteerAxleAhead = vehicleFile.value("IsSteerAxleAhead", DEFAULT_ISSTEERINGAXLEAHEAD).toBool();
    isToolBehindPivot = vehicleFile.value("IsToolBehindPivot", DEFAULT_ISTOOLBEHINDPIVOT).toBool();
    isToolTrailing = vehicleFile.value("IsToolTrailing", DEFAULT_ISTOOLTRAILING).toBool();

    numOfSections = vehicleFile.value("Sections", DEFAULT_NUMSECTIONS).toInt();
    toolWidth = vehicleFile.value("ToolWidth", DEFAULT_TOOLWIDTH).toDouble();

    slowSpeedCutoff = vehicleFile.value("SlowSpeedCutoff", DEFAULT_SLOWSPEEDCUTOFF).toDouble();
    toolMinUnappliedPixels = vehicleFile.value("ToolMinUnappliedPixels", DEFAULT_MINAPPLIED).toInt();

    goalPointLookAhead = vehicleFile.value("GoalPointLookAhead", DEFAULT_GOALPOINTLOOKAHEAD).toDouble();
    maxSteerAngle = vehicleFile.value("MaxSteerAngle", DEFAULT_MAXSTEERINGANGLE).toDouble();
    maxAngularVelocity = vehicleFile.value("MaxAngularVelocity", DEFAULT_MAXANGULARVELOCITY).toDouble();
}

void CVehicle::makeBuffers()
{
    //requires a valid OpenGL context
    QSettings settings;

    QVector<QVector3D> v;
    if (buffersCurrent) return;

    double trailingTank, trailingTool;

    //settings doesn't change trailing hitch length if set to rigid, so do it here
    if (isToolTrailing)
    {
        trailingTank = tankTrailingHitchLength;
        trailingTool = toolTrailingHitchLength;
    }
    else { trailingTank = 0; trailingTool = 0; }

    //there is a trailing tow between hitch
    if (tankTrailingHitchLength < -2.0 && isToolTrailing)
    {
        v.clear();
        v.append(QVector3D(0,trailingTank,0));
        v.append(QVector3D(0,0,0));

        if (trailingTankHitchBuffer.isCreated() ){
            trailingTankHitchBuffer.destroy();
        }
        trailingTankHitchBuffer.create();
        trailingTankHitchBuffer.bind();
        trailingTankHitchBuffer.allocate(v.data(),v.count() * sizeof(QVector3D));
        trailingTankHitchBuffer.release();

        v.clear();
        v.append(QVector3D(0,trailingTank,0));
        if (tankDotBuffer.isCreated())
            tankDotBuffer.destroy();
        tankDotBuffer.create();
        tankDotBuffer.allocate(v.data(),v.count() * sizeof(QVector3D));
        tankDotBuffer.release();
    }

    //draw the hitch if trailing
    if (isToolTrailing)
    {
        v.clear();
        v.append(QVector3D(0,trailingTool,0));
        v.append(QVector3D(0,0,0));

        if(toolHitchBuffer.isCreated())
            toolHitchBuffer.destroy();
        toolHitchBuffer.create();
        toolHitchBuffer.bind();
        toolHitchBuffer.allocate(v.data(),v.count() * sizeof(QVector3D));
        toolHitchBuffer.release();
    }

    //super section
    v.clear();
    v.append(QVector3D(section[numOfSections].positionLeft, trailingTool,0));
    v.append(QVector3D(section[numOfSections].positionRight, trailingTool,0));

    if (superSectionBuffer.isCreated())
        superSectionBuffer.destroy();
    superSectionBuffer.create();
    superSectionBuffer.bind();
    superSectionBuffer.allocate(v.data(),v.count() * sizeof(QVector3D));
    superSectionBuffer.release();

    // individual sections
    for (int j = 0; j < numOfSections; j++)
    {
        v.clear();
        v.append(QVector3D(section[j].positionLeft, trailingTool,0));
        v.append(QVector3D(section[j].positionRight, trailingTool,0));

        if (sectionBuffer[j].isCreated())
            sectionBuffer[j].destroy();
        sectionBuffer[j].create();
        sectionBuffer[j].bind();
        sectionBuffer[j].allocate(v.data(), v.count() * sizeof(QVector3D));
        sectionBuffer[j].release();
    }

    //section dots
    v.clear();
    for (int j = 0; j < numOfSections - 1; j++)
        v.append(QVector3D(section[j].positionRight, trailingTool,0));

    if (sectionDotsBuffer.isCreated())
        sectionDotsBuffer.destroy();
    sectionDotsBuffer.create();
    sectionDotsBuffer.bind();
    sectionDotsBuffer.allocate(v.data(), v.count() * sizeof(QVector3D));
    sectionDotsBuffer.release();

    //draw the vehicle Body
    ColorVertex vb[] = {
        { QVector3D(0,0,-0.2),                QVector4D(0.9, 0.5, 0.30, 1.0) },
        { QVector3D(1.8, -antennaPivot, 0.0), QVector4D(0.9, 0.5, 0.30, 1.0) },
        { QVector3D(0, -antennaPivot + wheelbase, 0.0), QVector4D(0.9, 0.5, 0.30, 1.0) },
        { QVector3D(-1.8, -antennaPivot, 0.0), QVector4D(0.20, 0.0, 0.9, 1.0) },
        { QVector3D(1.8, -antennaPivot, 0.0),  QVector4D(0.20, 0.0, 0.9, 1.0) }
    };

    if (vehicleBodyBuffer.isCreated())
        vehicleBodyBuffer.destroy();
    vehicleBodyBuffer.create();
    vehicleBodyBuffer.bind();
    vehicleBodyBuffer.allocate(vb,5 * sizeof(ColorVertex));
    vehicleBodyBuffer.release();

    //draw the area side marker, antenna, hitch pin ,etc
    ColorVertex m[] = {
        //area side marker
        { QVector3D(), QVector4D(0.95f, 0.90f, 0.0f, 1.0) },
        //antenna
        { QVector3D(0,0,0), QVector4D(0.0f, 0.98f, 0.0f, 1.0) },
        //hitch pin
        { QVector3D(0, hitchLength - antennaPivot, 0), QVector4D(0.99f, 0.0f, 0.0f, 1.0) }
    };
    if (settings.value("vehicle/isAreaOnRight",true).toBool())
        m[0].vertex = QVector3D(2.0, -antennaPivot, 0);
    else
        m[0].vertex = QVector3D(-2.0, -antennaPivot, 0);

    if (pinsAndMarkersBuffer.isCreated())
        pinsAndMarkersBuffer.destroy();
    pinsAndMarkersBuffer.create();
    pinsAndMarkersBuffer.bind();
    pinsAndMarkersBuffer.allocate(m,sizeof(ColorVertex)*3);
    pinsAndMarkersBuffer.release();

    //yellow pointer
    v.clear();
    v.append(QVector3D(1.2, -antennaPivot + wheelbase + 5, 0.0));
    v.append(QVector3D(0, -antennaPivot + wheelbase + 10, 0.0));
    v.append(QVector3D(-1.2, -antennaPivot + wheelbase + 5, 0.0));

    if (pointerBuffer.isCreated())
        pointerBuffer.destroy();
    pointerBuffer.create();
    pointerBuffer.bind();
    pointerBuffer.allocate(v.data(),v.count()*sizeof(QVector3D));
    pointerBuffer.release();

    //draw the rigid hitch

    v.clear();
    v.append(QVector3D(0, float(hitchLength - antennaPivot), 0));
    v.append(QVector3D(0, float(-antennaPivot), 0));

    if (rigidHitchBuffer.isCreated())
        rigidHitchBuffer.destroy();
    rigidHitchBuffer.create();
    rigidHitchBuffer.bind();
    rigidHitchBuffer.allocate(v.data(),v.count()*sizeof(QVector3D));
    rigidHitchBuffer.release();


    buffersCurrent = true;
}

void CVehicle::drawVehicle(QOpenGLContext *glContext, QMatrix4x4 &modelview,
                           const QMatrix4x4 &projection, bool drawSectionMarkers) {
    QOpenGLFunctions *gl =glContext->functions();
    //QOpenGLFunctions_2_1 *gl21 = glContext->versionFunctions<QOpenGLFunctions_2_1>();

    QColor color;
    makeBuffers(); //create openGL buffers if necessary.

    //translate and rotate at pivot axle
    //this will change the modelview for our caller, which is what
    //was happening here.
    //gl21->glTranslated(mf->fixEasting, mf->fixNorthing, 0);
    modelview.translate(float(fixEasting), float(fixNorthing), 0);

    //gl->glPushMatrix();
    QMatrix4x4 mvTool = modelview;

    //most complicated translate ever!
    //gl->glTranslated((sin(mf->fixHeading) * (hitchLength - antennaPivot)),
    //                (cos(mf->fixHeading) * (hitchLength - antennaPivot)), 0);
    mvTool.translate(float(sin(fixHeading) * (hitchLength - antennaPivot)),
                     float(cos(fixHeading) * (hitchLength - antennaPivot)), 0);

    //settings doesn't change trailing hitch length if set to rigid, so do it here
    double trailingTank;
    if (isToolTrailing)
    {
        trailingTank = tankTrailingHitchLength;
    }
    else { trailingTank = 0;  }

    //there is a trailing tow between hitch
    if (tankTrailingHitchLength < -2.0 && isToolTrailing)
    {
        //gl->glRotated(toDegrees(-mf->fixHeadingTank), 0.0, 0.0, 1.0);
        mvTool.rotate(glm::toDegrees(float(-fixHeadingTank)), 0.0, 0.0, 1.0);

        //draw the tank hitch
        gl->glLineWidth(2);

        //gl->glColor3f(0.7f, 0.7f, 0.97f);
        color.setRgbF(0.7,0.7, 0.97);
        //gl->glBegin(GL_LINES);
        //gl->glVertex3d(0, trailingTank, 0);
        //gl->glVertex3d(0, 0, 0);
        //gl->glEnd();
        glDrawArraysColor(gl,projection*mvTool,
                          GL_LINES, color,
                          trailingTankHitchBuffer,GL_FLOAT,
                          2);


        //gl->glColor3f(0.95f, 0.950f, 0.0f);
        color.setRgbF(0.95, 0.95, 0.0);
        //gl->glPointSize(6.0f);
        //gl->glBegin(GL_POINTS);
        //gl->glVertex3d(0, trailingTank, 0);
        //gl->glEnd();
        glDrawArraysColor(gl,projection*mvTool,
                          GL_POINTS,color,
                          tankDotBuffer, GL_FLOAT,
                          1, 6.0f);



        //move down the tank hitch, unwind, rotate to section heading
        //gl->glTranslated(0, trailingTank, 0);
        //gl->glRotated(toDegrees(mf->fixHeadingTank), 0.0, 0.0, 1.0);
        //gl->glRotated(toDegrees(-mf->fixHeadingSection), 0.0, 0.0, 1.0);

        mvTool.translate(0, float(trailingTank), 0);
        mvTool.rotate(glm::toDegrees(float(fixHeadingTank)), 0.0, 0.0, 1.0);
        mvTool.rotate(glm::toDegrees(float(-fixHeadingSection)), 0.0, 0.0, 1.0);
    }
    //no tow between hitch
    else {
        //gl->glRotated(toDegrees(-mf->fixHeadingSection), 0.0, 0.0, 1.0);
        mvTool.rotate(glm::toDegrees(float(-fixHeadingSection)), 0.0, 0.0, 1.0);
    }

    //draw the hitch if trailing
    if (isToolTrailing)
    {
        gl->glLineWidth(2);
        //gl->glColor3f(0.7f, 0.7f, 0.97f);
        color.setRgbF(0.7, 0.7, 0.97);
        //gl->glBegin(GL_LINES);
        //gl->glVertex3d(0, trailingTool, 0);
        //gl->glVertex3d(0, 0, 0);
        //gl->glEnd();
        glDrawArraysColor(gl,projection*mvTool,
                          GL_LINES, color,
                          toolHitchBuffer,GL_FLOAT,
                          2);

    }

    //draw the sections
    gl->glLineWidth(8);

    //gl->glBegin(GL_LINES);
    //draw section line.  This is the single big section
    if (section[numOfSections].isSectionOn)
    {
        if (section[0].manBtnState == btnStates::Auto)
            //gl->glColor3f(0.0f, 0.97f, 0.0f);
            color.setRgbF(0.0,0.97,0.0);
        else
            //gl->glColor3f(0.99, 0.99, 0);
            color.setRgbF(0.99, 0.99, 0.0);

        //gl->glVertex3d(mf->section[numOfSections].positionLeft, trailingTool, 0);
        //gl->glVertex3d(mf->section[numOfSections].positionRight, trailingTool, 0);
        glDrawArraysColor(gl,projection*mvTool,
                          GL_LINES,color,
                          superSectionBuffer,GL_FLOAT,
                          2);
    } else {
        for (int j = 0; j < numOfSections; j++)
        {
            //if section is on, green, if off, red color
            if (section[j].isSectionOn)
            {
                if (section[j].manBtnState == btnStates::Auto)
                    //gl->glColor3f(0.0f, 0.97f, 0.0f);
                    color.setRgbF(0.0, 0.97, 0.0);
                else
                    //gl->glColor3f(0.97, 0.97, 0);
                    color.setRgbF(0.97, 0.97, 0.0);
            } else
                //gl->glColor3f(0.97f, 0.2f, 0.2f);
                color.setRgbF(0.97, 0.2, 0.2);

            //draw section line
            //gl->glVertex3d(mf->section[j].positionLeft, trailingTool, 0);
            //gl->glVertex3d(mf->section[j].positionRight, trailingTool, 0);
            glDrawArraysColor(gl,projection*mvTool,
                              GL_LINES,color,
                              sectionBuffer[j],GL_FLOAT,
                              2);
        }
    }
    //gl->glEnd();


    //draw section markers if close enough
    if (drawSectionMarkers)
    {
        //gl->glColor3f(0.0f, 0.0f, 0.0f);
        color.setRgbF(0.0, 0.0, 0.0);

        //section markers
        //TODO: this needs to go in the shder
        //gl->glPointSize(4.0f);

        //gl->glBegin(GL_POINTS);
        glDrawArraysColor(gl,projection*mvTool,
                          GL_POINTS,color,
                          sectionDotsBuffer,GL_FLOAT,
                          numOfSections - 1,
                          4.0);
        //gl->glEnd();
    }

    //draw vehicle
    //gl21->glPopMatrix();

    //gl21->glRotated(toDegrees(-mf->fixHeading), 0.0, 0.0, 1.0);
    modelview.rotate(glm::toDegrees(-fixHeading), 0.0, 0.0, 1.0);

    //draw the vehicle Body
    //gl21->glColor3f(0.9, 0.5, 0.30);
    //gl21->glBegin(GL_TRIANGLE_FAN);

    //gl21->glVertex3d(0, 0, -0.2);
    //gl21->glVertex3d(1.8, -antennaPivot, 0.0);
    //gl21->glVertex3d(0, -antennaPivot + wheelbase, 0.0);
    //gl21->glColor3f(0.20, 0.0, 0.9);
    //gl21->glVertex3d(-1.8, -antennaPivot, 0.0);
    //gl21->glVertex3d(1.8, -antennaPivot, 0.0);
    //gl21->glEnd();
    glDrawArraysColors(gl,projection*modelview,
                       GL_TRIANGLE_FAN,
                       vehicleBodyBuffer,
                       GL_FLOAT,
                       5); //5 vertices in body

    /*
    //draw the area side marker
    gl21->glColor3f(0.95f, 0.90f, 0.0f);
    gl21->glPointSize(4.0f);
    gl21->glBegin(GL_POINTS);
    if (mf->isAreaOnRight) gl21->glVertex3d(2.0, -antennaPivot, 0);
    else gl21->glVertex3d(-2.0, -antennaPivot, 0);

    //antenna
    gl21->glColor3f(0.0f, 0.98f, 0.0f);
    gl21->glVertex3d(0, 0, 0);

    //hitch pin
    gl21->glColor3f(0.99f, 0.0f, 0.0f);
    gl21->glVertex3d(0, hitchLength - antennaPivot, 0);

    ////rear Tires
    //glPointSize(12.0f);
    //glColor3f(0, 0, 0);
    //glVertex3d(-1.8, 0, -antennaPivot);
    //glVertex3d(1.8, 0, -antennaPivot);
    gl21->glEnd();
    */
    //draw the area side marker, antenna, hitch pin ,etc
    glDrawArraysColors(gl,projection*modelview,
                       GL_POINTS,
                       pinsAndMarkersBuffer,
                       GL_FLOAT,
                       3,
                       4.0f);

    //gl21->glColor3f(0.9, 0.95, 0.10);
    //gl21->glBegin(GL_LINE_STRIP);
    //{
    //    gl21->glVertex3d(1.2, -antennaPivot + wheelbase + 5, 0.0);
    //    gl21->glVertex3d(0, -antennaPivot + wheelbase + 10, 0.0);
    //    gl21->glVertex3d(-1.2, -antennaPivot + wheelbase + 5, 0.0);
    //}
    //gl21->glEnd();

    gl->glLineWidth(1);
    color.setRgbF(0.9, 0.95, 0.10);
    glDrawArraysColor(gl, projection * modelview,
                      GL_LINE_STRIP, color,
                      pointerBuffer,
                      GL_FLOAT,
                      3);



    //draw the rigid hitch
    //gl21->glColor3f(0.37f, 0.37f, 0.97f);
    //gl21->glBegin(GL_LINES);
    //gl21->glVertex3d(0, hitchLength - antennaPivot, 0);
    //gl21->glVertex3d(0, -antennaPivot, 0);
    //gl21->glEnd();
    color.setRgbF(0.37, 0.37, 0.97);
    glDrawArraysColor(gl, projection * modelview,
                      GL_LINES, color,
                      rigidHitchBuffer,
                      GL_FLOAT,
                      2);
}

void CVehicle::destroyGLBuffers()
{
    //assume valid OpenGL context

    trailingTankHitchBuffer.destroy();
    tankDotBuffer.destroy();
    toolHitchBuffer.destroy();
    superSectionBuffer.destroy();

    for (int j = 0; j <= MAXSECTIONS; j++)
        sectionBuffer[j].destroy();

    sectionDotsBuffer.destroy();
    vehicleBodyBuffer.destroy();
    pinsAndMarkersBuffer.destroy();
    pointerBuffer.destroy();
    rigidHitchBuffer.destroy();
}

void CVehicle::settingsChanged()
{
    //regenerate buffers since settings have changed.
    buffersCurrent = false;
}

//function to calculate the width of each section and update
void CVehicle::sectionCalcWidths()
{
    for (int j = 0; j < MAXSECTIONS; j++)
    {
        section[j].sectionWidth = (section[j].positionRight - section[j].positionLeft);
        section[j].rpSectionPosition = 200 + int(glm::roundAwayFromZero(section[j].positionLeft * 10));
        section[j].rpSectionWidth = int(glm::roundAwayFromZero(section[j].sectionWidth * 10));
    }

    //calculate tool width based on extreme right and left values
    toolWidth = fabs(section[0].positionLeft) + fabs(section[numOfSections - 1].positionRight);

    //left and right tool position
    toolFarLeftPosition = section[0].positionLeft;
    toolFarRightPosition = section[numOfSections - 1].positionRight;

    //now do the full width section
    section[numOfSections].sectionWidth = toolWidth;
    section[numOfSections].positionLeft = toolFarLeftPosition;
    section[numOfSections].positionRight = toolFarRightPosition;

    //find the right side pixel position
    rpXPosition = 200 + int(glm::roundAwayFromZero(toolFarLeftPosition * 10));
    rpWidth = int(glm::roundAwayFromZero(toolWidth * 10));
}
