/********************************************************************************
** Form generated from reading UI file 'v3d.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_V3D_H
#define UI_V3D_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSlider>
#include <QtWidgets/QWidget>
#include "light3d.h"

QT_BEGIN_NAMESPACE

class Ui_V3DClass
{
public:
    QCheckBox *useTFColors;
    QCheckBox *HDRcheckBox;
    QScrollArea *mainSettingsDesk;
    QWidget *scrollAreaWidgetContents;
    QGroupBox *groupBox_4;
    light3D *light3d;
    QGroupBox *groupBox_2;
    QLabel *ambiemtIL;
    QSlider *ambientSlider;
    QPushButton *ambientButton;
    QLabel *diffuseIL;
    QSlider *diffuseSlider;
    QPushButton *diffuseButton;
    QLabel *specularIL;
    QSlider *specularSlider;
    QPushButton *specularButton;
    QLabel *distanceL;
    QSlider *distanceSlider;
    QGroupBox *groupBox;
    QSlider *exposureSlider;
    QLabel *exposureL;
    QGroupBox *groupBox_8;
    QSlider *gammaCorrectionSlider;
    QLabel *gammaCorrectionL;
    QGroupBox *groupBox_5;
    QSlider *surfaceShadingParameter;
    QLabel *SSPL;
    QGroupBox *groupBox_7;
    QLabel *colorBox;
    QSlider *colorBoxSlider;
    QLabel *colorBoxL;
    QGroupBox *groupBox_3;
    QSlider *samplingCount;
    QLabel *sampleCL;
    QSlider *gradientCount;
    QLabel *gradientCL;
    QGroupBox *groupBox_6;
    QScrollArea *illuminationController;
    QWidget *scrollAreaWidgetContents_2;
    QRadioButton *noLightCheck;
    QRadioButton *inDirectPhongCheck;
    QLabel *lightAttenuationL;
    QSlider *lightAttenuationSlider;
    QLabel *Label_4;
    QSlider *scatteringCoeffSlider;
    QRadioButton *localLightCheck;
    QRadioButton *directIlluminationCheck;
    QRadioButton *indirectIlluminationCheck;
    QRadioButton *globalIlluminationCheck;
    QRadioButton *directPhongIlluminationCheck;
    QRadioButton *DirectIndirectPhongCheck;
    QCheckBox *autoRotate;

    void setupUi(QWidget *V3DClass)
    {
        if (V3DClass->objectName().isEmpty())
            V3DClass->setObjectName(QStringLiteral("V3DClass"));
        V3DClass->setEnabled(true);
        V3DClass->resize(1039, 744);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(V3DClass->sizePolicy().hasHeightForWidth());
        V3DClass->setSizePolicy(sizePolicy);
        V3DClass->setMinimumSize(QSize(0, 0));
        V3DClass->setMaximumSize(QSize(16777215, 16777215));
        V3DClass->setFocusPolicy(Qt::NoFocus);
        QIcon icon;
        icon.addFile(QStringLiteral("Resources/images.jpg"), QSize(), QIcon::Normal, QIcon::Off);
        V3DClass->setWindowIcon(icon);
        V3DClass->setStyleSheet(QLatin1String("QSlider::groove:horizontal\n"
"{\n"
"	border:0px;height:4px;\n"
"}\n"
"QSlider::sub-page:horizontal\n"
"{\n"
"	background:#808080;\n"
"} \n"
"QSlider::add-page:horizontal\n"
"{\n"
"	background:lightgray;\n"
"}\n"
"QSlider::handle:horizontal\n"
"{\n"
"	background:#808080;width:10px;border-radius:5px;margin:-3px 0px -3px 0px;\n"
"}\n"
"QScrollArea{border: 0 px}"));
        useTFColors = new QCheckBox(V3DClass);
        useTFColors->setObjectName(QStringLiteral("useTFColors"));
        useTFColors->setGeometry(QRect(30, 10, 101, 31));
        QFont font;
        font.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        useTFColors->setFont(font);
        useTFColors->setChecked(true);
        HDRcheckBox = new QCheckBox(V3DClass);
        HDRcheckBox->setObjectName(QStringLiteral("HDRcheckBox"));
        HDRcheckBox->setGeometry(QRect(30, 40, 91, 16));
        HDRcheckBox->setFont(font);
        HDRcheckBox->setChecked(true);
        mainSettingsDesk = new QScrollArea(V3DClass);
        mainSettingsDesk->setObjectName(QStringLiteral("mainSettingsDesk"));
        mainSettingsDesk->setGeometry(QRect(440, 10, 411, 711));
        mainSettingsDesk->setMinimumSize(QSize(300, 0));
        mainSettingsDesk->setMaximumSize(QSize(16777215, 16777215));
        mainSettingsDesk->setStyleSheet(QStringLiteral(""));
        mainSettingsDesk->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 394, 1150));
        scrollAreaWidgetContents->setMinimumSize(QSize(327, 1150));
        groupBox_4 = new QGroupBox(scrollAreaWidgetContents);
        groupBox_4->setObjectName(QStringLiteral("groupBox_4"));
        groupBox_4->setGeometry(QRect(10, 0, 161, 161));
        groupBox_4->setFont(font);
        light3d = new light3D(groupBox_4);
        light3d->setObjectName(QStringLiteral("light3d"));
        light3d->setGeometry(QRect(10, 20, 141, 131));
        groupBox_2 = new QGroupBox(scrollAreaWidgetContents);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setGeometry(QRect(10, 480, 381, 191));
        groupBox_2->setFont(font);
        ambiemtIL = new QLabel(groupBox_2);
        ambiemtIL->setObjectName(QStringLiteral("ambiemtIL"));
        ambiemtIL->setGeometry(QRect(20, 20, 191, 16));
        ambiemtIL->setFont(font);
        ambientSlider = new QSlider(groupBox_2);
        ambientSlider->setObjectName(QStringLiteral("ambientSlider"));
        ambientSlider->setGeometry(QRect(20, 40, 341, 19));
        ambientSlider->setFocusPolicy(Qt::NoFocus);
        ambientSlider->setMaximum(100);
        ambientSlider->setSingleStep(1);
        ambientSlider->setPageStep(10);
        ambientSlider->setValue(0);
        ambientSlider->setOrientation(Qt::Horizontal);
        ambientButton = new QPushButton(groupBox_2);
        ambientButton->setObjectName(QStringLiteral("ambientButton"));
        ambientButton->setGeometry(QRect(160, 20, 21, 20));
        ambientButton->setStyleSheet(QStringLiteral("background-color: rgb(255, 255, 255);"));
        diffuseIL = new QLabel(groupBox_2);
        diffuseIL->setObjectName(QStringLiteral("diffuseIL"));
        diffuseIL->setGeometry(QRect(20, 60, 171, 16));
        diffuseIL->setFont(font);
        diffuseSlider = new QSlider(groupBox_2);
        diffuseSlider->setObjectName(QStringLiteral("diffuseSlider"));
        diffuseSlider->setGeometry(QRect(20, 80, 351, 19));
        diffuseSlider->setFocusPolicy(Qt::NoFocus);
        diffuseSlider->setMaximum(100);
        diffuseSlider->setValue(100);
        diffuseSlider->setOrientation(Qt::Horizontal);
        diffuseButton = new QPushButton(groupBox_2);
        diffuseButton->setObjectName(QStringLiteral("diffuseButton"));
        diffuseButton->setGeometry(QRect(160, 60, 21, 21));
        diffuseButton->setStyleSheet(QStringLiteral("background-color: rgb(255, 255, 255);"));
        specularIL = new QLabel(groupBox_2);
        specularIL->setObjectName(QStringLiteral("specularIL"));
        specularIL->setGeometry(QRect(20, 100, 181, 16));
        specularIL->setFont(font);
        specularSlider = new QSlider(groupBox_2);
        specularSlider->setObjectName(QStringLiteral("specularSlider"));
        specularSlider->setGeometry(QRect(20, 120, 351, 19));
        specularSlider->setFocusPolicy(Qt::NoFocus);
        specularSlider->setMaximum(100);
        specularSlider->setOrientation(Qt::Horizontal);
        specularButton = new QPushButton(groupBox_2);
        specularButton->setObjectName(QStringLiteral("specularButton"));
        specularButton->setGeometry(QRect(160, 100, 21, 20));
        specularButton->setStyleSheet(QStringLiteral("background-color: rgb(255, 255, 255);"));
        distanceL = new QLabel(groupBox_2);
        distanceL->setObjectName(QStringLiteral("distanceL"));
        distanceL->setGeometry(QRect(20, 140, 141, 16));
        distanceL->setFont(font);
        distanceSlider = new QSlider(groupBox_2);
        distanceSlider->setObjectName(QStringLiteral("distanceSlider"));
        distanceSlider->setGeometry(QRect(20, 160, 351, 19));
        distanceSlider->setFocusPolicy(Qt::NoFocus);
        distanceSlider->setMinimum(0);
        distanceSlider->setMaximum(100);
        distanceSlider->setValue(25);
        distanceSlider->setOrientation(Qt::Horizontal);
        groupBox = new QGroupBox(scrollAreaWidgetContents);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(10, 680, 301, 71));
        groupBox->setFont(font);
        exposureSlider = new QSlider(groupBox);
        exposureSlider->setObjectName(QStringLiteral("exposureSlider"));
        exposureSlider->setGeometry(QRect(20, 40, 271, 20));
        exposureSlider->setFocusPolicy(Qt::NoFocus);
        exposureSlider->setMinimum(0);
        exposureSlider->setMaximum(50);
        exposureSlider->setSingleStep(2);
        exposureSlider->setPageStep(2);
        exposureSlider->setValue(25);
        exposureSlider->setOrientation(Qt::Horizontal);
        exposureL = new QLabel(groupBox);
        exposureL->setObjectName(QStringLiteral("exposureL"));
        exposureL->setGeometry(QRect(20, 20, 151, 16));
        exposureL->setFont(font);
        groupBox_8 = new QGroupBox(scrollAreaWidgetContents);
        groupBox_8->setObjectName(QStringLiteral("groupBox_8"));
        groupBox_8->setGeometry(QRect(10, 760, 301, 71));
        groupBox_8->setFont(font);
        gammaCorrectionSlider = new QSlider(groupBox_8);
        gammaCorrectionSlider->setObjectName(QStringLiteral("gammaCorrectionSlider"));
        gammaCorrectionSlider->setGeometry(QRect(20, 40, 271, 20));
        gammaCorrectionSlider->setFocusPolicy(Qt::NoFocus);
        gammaCorrectionSlider->setMinimum(1);
        gammaCorrectionSlider->setMaximum(26);
        gammaCorrectionSlider->setSingleStep(1);
        gammaCorrectionSlider->setPageStep(1);
        gammaCorrectionSlider->setValue(16);
        gammaCorrectionSlider->setOrientation(Qt::Horizontal);
        gammaCorrectionL = new QLabel(groupBox_8);
        gammaCorrectionL->setObjectName(QStringLiteral("gammaCorrectionL"));
        gammaCorrectionL->setGeometry(QRect(20, 20, 131, 16));
        gammaCorrectionL->setFont(font);
        groupBox_5 = new QGroupBox(scrollAreaWidgetContents);
        groupBox_5->setObjectName(QStringLiteral("groupBox_5"));
        groupBox_5->setGeometry(QRect(10, 840, 301, 71));
        groupBox_5->setFont(font);
        surfaceShadingParameter = new QSlider(groupBox_5);
        surfaceShadingParameter->setObjectName(QStringLiteral("surfaceShadingParameter"));
        surfaceShadingParameter->setGeometry(QRect(20, 40, 271, 19));
        surfaceShadingParameter->setFocusPolicy(Qt::NoFocus);
        surfaceShadingParameter->setMinimum(0);
        surfaceShadingParameter->setMaximum(100);
        surfaceShadingParameter->setSingleStep(10);
        surfaceShadingParameter->setValue(100);
        surfaceShadingParameter->setOrientation(Qt::Horizontal);
        SSPL = new QLabel(groupBox_5);
        SSPL->setObjectName(QStringLiteral("SSPL"));
        SSPL->setGeometry(QRect(20, 20, 211, 20));
        SSPL->setFont(font);
        groupBox_7 = new QGroupBox(scrollAreaWidgetContents);
        groupBox_7->setObjectName(QStringLiteral("groupBox_7"));
        groupBox_7->setGeometry(QRect(10, 920, 221, 81));
        groupBox_7->setFont(font);
        colorBox = new QLabel(groupBox_7);
        colorBox->setObjectName(QStringLiteral("colorBox"));
        colorBox->setGeometry(QRect(10, 50, 201, 21));
        colorBox->setStyleSheet(QStringLiteral("image: url(:/V3D/Resources/colobox.png);"));
        colorBoxSlider = new QSlider(groupBox_7);
        colorBoxSlider->setObjectName(QStringLiteral("colorBoxSlider"));
        colorBoxSlider->setGeometry(QRect(10, 30, 201, 19));
        colorBoxSlider->setStyleSheet(QStringLiteral(""));
        colorBoxSlider->setMaximum(200);
        colorBoxSlider->setPageStep(1);
        colorBoxSlider->setValue(100);
        colorBoxSlider->setOrientation(Qt::Horizontal);
        colorBoxL = new QLabel(groupBox_7);
        colorBoxL->setObjectName(QStringLiteral("colorBoxL"));
        colorBoxL->setGeometry(QRect(10, 20, 31, 16));
        groupBox_3 = new QGroupBox(scrollAreaWidgetContents);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        groupBox_3->setGeometry(QRect(10, 1010, 381, 121));
        groupBox_3->setFont(font);
        samplingCount = new QSlider(groupBox_3);
        samplingCount->setObjectName(QStringLiteral("samplingCount"));
        samplingCount->setGeometry(QRect(20, 40, 351, 20));
        samplingCount->setMinimum(100);
        samplingCount->setMaximum(3000);
        samplingCount->setSingleStep(1);
        samplingCount->setValue(800);
        samplingCount->setSliderPosition(800);
        samplingCount->setOrientation(Qt::Horizontal);
        sampleCL = new QLabel(groupBox_3);
        sampleCL->setObjectName(QStringLiteral("sampleCL"));
        sampleCL->setGeometry(QRect(20, 20, 131, 16));
        sampleCL->setFont(font);
        gradientCount = new QSlider(groupBox_3);
        gradientCount->setObjectName(QStringLiteral("gradientCount"));
        gradientCount->setGeometry(QRect(20, 80, 351, 20));
        gradientCount->setMinimum(100);
        gradientCount->setMaximum(3000);
        gradientCount->setSingleStep(1);
        gradientCount->setValue(800);
        gradientCount->setOrientation(Qt::Horizontal);
        gradientCL = new QLabel(groupBox_3);
        gradientCL->setObjectName(QStringLiteral("gradientCL"));
        gradientCL->setGeometry(QRect(20, 60, 131, 16));
        gradientCL->setFont(font);
        groupBox_6 = new QGroupBox(scrollAreaWidgetContents);
        groupBox_6->setObjectName(QStringLiteral("groupBox_6"));
        groupBox_6->setGeometry(QRect(10, 170, 221, 301));
        groupBox_6->setFont(font);
        illuminationController = new QScrollArea(groupBox_6);
        illuminationController->setObjectName(QStringLiteral("illuminationController"));
        illuminationController->setGeometry(QRect(20, 30, 181, 261));
        illuminationController->setMinimumSize(QSize(0, 0));
        illuminationController->setMaximumSize(QSize(16777215, 16777215));
        illuminationController->setStyleSheet(QStringLiteral("QScrollArea{border:0px solid #555;}"));
        illuminationController->setWidgetResizable(true);
        scrollAreaWidgetContents_2 = new QWidget();
        scrollAreaWidgetContents_2->setObjectName(QStringLiteral("scrollAreaWidgetContents_2"));
        scrollAreaWidgetContents_2->setGeometry(QRect(0, 0, 164, 400));
        scrollAreaWidgetContents_2->setMinimumSize(QSize(0, 400));
        noLightCheck = new QRadioButton(scrollAreaWidgetContents_2);
        noLightCheck->setObjectName(QStringLiteral("noLightCheck"));
        noLightCheck->setGeometry(QRect(0, -10, 131, 31));
        noLightCheck->setFont(font);
        noLightCheck->setChecked(false);
        inDirectPhongCheck = new QRadioButton(scrollAreaWidgetContents_2);
        inDirectPhongCheck->setObjectName(QStringLiteral("inDirectPhongCheck"));
        inDirectPhongCheck->setGeometry(QRect(0, 50, 201, 31));
        inDirectPhongCheck->setFont(font);
        inDirectPhongCheck->setChecked(true);
        lightAttenuationL = new QLabel(scrollAreaWidgetContents_2);
        lightAttenuationL->setObjectName(QStringLiteral("lightAttenuationL"));
        lightAttenuationL->setGeometry(QRect(0, 240, 121, 16));
        lightAttenuationL->setFont(font);
        lightAttenuationSlider = new QSlider(scrollAreaWidgetContents_2);
        lightAttenuationSlider->setObjectName(QStringLiteral("lightAttenuationSlider"));
        lightAttenuationSlider->setGeometry(QRect(0, 270, 201, 19));
        lightAttenuationSlider->setFocusPolicy(Qt::NoFocus);
        lightAttenuationSlider->setMinimum(0);
        lightAttenuationSlider->setMaximum(40);
        lightAttenuationSlider->setSingleStep(1);
        lightAttenuationSlider->setPageStep(1);
        lightAttenuationSlider->setValue(0);
        lightAttenuationSlider->setOrientation(Qt::Horizontal);
        Label_4 = new QLabel(scrollAreaWidgetContents_2);
        Label_4->setObjectName(QStringLiteral("Label_4"));
        Label_4->setGeometry(QRect(0, 300, 121, 16));
        Label_4->setFont(font);
        scatteringCoeffSlider = new QSlider(scrollAreaWidgetContents_2);
        scatteringCoeffSlider->setObjectName(QStringLiteral("scatteringCoeffSlider"));
        scatteringCoeffSlider->setGeometry(QRect(0, 330, 201, 19));
        scatteringCoeffSlider->setFocusPolicy(Qt::NoFocus);
        scatteringCoeffSlider->setMinimum(0);
        scatteringCoeffSlider->setMaximum(255);
        scatteringCoeffSlider->setSingleStep(1);
        scatteringCoeffSlider->setPageStep(1);
        scatteringCoeffSlider->setValue(0);
        scatteringCoeffSlider->setOrientation(Qt::Horizontal);
        localLightCheck = new QRadioButton(scrollAreaWidgetContents_2);
        localLightCheck->setObjectName(QStringLiteral("localLightCheck"));
        localLightCheck->setGeometry(QRect(0, 10, 181, 51));
        localLightCheck->setFont(font);
        localLightCheck->setChecked(false);
        localLightCheck->setAutoRepeat(false);
        directIlluminationCheck = new QRadioButton(scrollAreaWidgetContents_2);
        directIlluminationCheck->setObjectName(QStringLiteral("directIlluminationCheck"));
        directIlluminationCheck->setGeometry(QRect(0, 80, 181, 31));
        directIlluminationCheck->setFont(font);
        directIlluminationCheck->setChecked(false);
        indirectIlluminationCheck = new QRadioButton(scrollAreaWidgetContents_2);
        indirectIlluminationCheck->setObjectName(QStringLiteral("indirectIlluminationCheck"));
        indirectIlluminationCheck->setGeometry(QRect(0, 110, 181, 31));
        indirectIlluminationCheck->setFont(font);
        indirectIlluminationCheck->setChecked(false);
        globalIlluminationCheck = new QRadioButton(scrollAreaWidgetContents_2);
        globalIlluminationCheck->setObjectName(QStringLiteral("globalIlluminationCheck"));
        globalIlluminationCheck->setGeometry(QRect(0, 140, 211, 31));
        globalIlluminationCheck->setFont(font);
        globalIlluminationCheck->setChecked(false);
        directPhongIlluminationCheck = new QRadioButton(scrollAreaWidgetContents_2);
        directPhongIlluminationCheck->setObjectName(QStringLiteral("directPhongIlluminationCheck"));
        directPhongIlluminationCheck->setGeometry(QRect(0, 170, 191, 31));
        directPhongIlluminationCheck->setFont(font);
        directPhongIlluminationCheck->setChecked(false);
        DirectIndirectPhongCheck = new QRadioButton(scrollAreaWidgetContents_2);
        DirectIndirectPhongCheck->setObjectName(QStringLiteral("DirectIndirectPhongCheck"));
        DirectIndirectPhongCheck->setGeometry(QRect(0, 200, 201, 31));
        DirectIndirectPhongCheck->setFont(font);
        DirectIndirectPhongCheck->setChecked(false);
        illuminationController->setWidget(scrollAreaWidgetContents_2);
        mainSettingsDesk->setWidget(scrollAreaWidgetContents);
        autoRotate = new QCheckBox(V3DClass);
        autoRotate->setObjectName(QStringLiteral("autoRotate"));
        autoRotate->setGeometry(QRect(30, 60, 91, 16));
        autoRotate->setFont(font);
        autoRotate->setChecked(false);

        retranslateUi(V3DClass);

        QMetaObject::connectSlotsByName(V3DClass);
    } // setupUi

    void retranslateUi(QWidget *V3DClass)
    {
        V3DClass->setWindowTitle(QApplication::translate("V3DClass", "3D Ultrasound Visualization Platform", nullptr));
#ifndef QT_NO_TOOLTIP
        V3DClass->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        useTFColors->setText(QApplication::translate("V3DClass", "COLOR", nullptr));
        HDRcheckBox->setText(QApplication::translate("V3DClass", "HDR", nullptr));
        groupBox_4->setTitle(QApplication::translate("V3DClass", "Light Direction", nullptr));
        groupBox_2->setTitle(QApplication::translate("V3DClass", "Lighting Source", nullptr));
        ambiemtIL->setText(QApplication::translate("V3DClass", "Ambient Instenity 0.0", nullptr));
#ifndef QT_NO_TOOLTIP
        ambientButton->setToolTip(QString());
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        ambientButton->setWhatsThis(QApplication::translate("V3DClass", "Ambient Light Color", nullptr));
#endif // QT_NO_WHATSTHIS
        ambientButton->setText(QString());
        diffuseIL->setText(QApplication::translate("V3DClass", "Diffuse Instenity 1.0", nullptr));
#ifndef QT_NO_TOOLTIP
        diffuseButton->setToolTip(QString());
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        diffuseButton->setWhatsThis(QApplication::translate("V3DClass", "Diffuse Light Color", nullptr));
#endif // QT_NO_WHATSTHIS
        diffuseButton->setText(QString());
        specularIL->setText(QApplication::translate("V3DClass", "Specular Instenity 0.0", nullptr));
#ifndef QT_NO_TOOLTIP
        specularButton->setToolTip(QString());
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        specularButton->setWhatsThis(QApplication::translate("V3DClass", "Specular Light Color", nullptr));
#endif // QT_NO_WHATSTHIS
        specularButton->setText(QString());
        distanceL->setText(QApplication::translate("V3DClass", "Distance 2.5", nullptr));
        groupBox->setTitle(QApplication::translate("V3DClass", "HDR", nullptr));
        exposureL->setText(QApplication::translate("V3DClass", "HDR Exposure 2.5", nullptr));
        groupBox_8->setTitle(QApplication::translate("V3DClass", "Gamma", nullptr));
        gammaCorrectionL->setText(QApplication::translate("V3DClass", "Gamma Correction 1.6", nullptr));
        groupBox_5->setTitle(QApplication::translate("V3DClass", "Surface Parameter", nullptr));
        SSPL->setText(QApplication::translate("V3DClass", "Surface Shading Parameter 1.0", nullptr));
        groupBox_7->setTitle(QApplication::translate("V3DClass", "Color Box", nullptr));
        colorBox->setText(QString());
        colorBoxL->setText(QApplication::translate("V3DClass", "0.0", nullptr));
        groupBox_3->setTitle(QApplication::translate("V3DClass", "Sampling", nullptr));
        sampleCL->setText(QApplication::translate("V3DClass", "Sample Count  800", nullptr));
        gradientCL->setText(QApplication::translate("V3DClass", "Gradient Count 800", nullptr));
        groupBox_6->setTitle(QApplication::translate("V3DClass", "Lighting Controller", nullptr));
        noLightCheck->setText(QApplication::translate("V3DClass", "Not Illumination", nullptr));
        inDirectPhongCheck->setText(QApplication::translate("V3DClass", "Indirect Illumination(Phong)", nullptr));
        lightAttenuationL->setText(QApplication::translate("V3DClass", "Light Attenuation 0.0", nullptr));
        Label_4->setText(QApplication::translate("V3DClass", "Scattering Coeff 0.0", nullptr));
        localLightCheck->setText(QApplication::translate("V3DClass", "Local Illumination(Phong)", nullptr));
        directIlluminationCheck->setText(QApplication::translate("V3DClass", "Direct Illumination", nullptr));
        indirectIlluminationCheck->setText(QApplication::translate("V3DClass", "Indirect Illumination", nullptr));
        globalIlluminationCheck->setText(QApplication::translate("V3DClass", "Global Illumination", nullptr));
        directPhongIlluminationCheck->setText(QApplication::translate("V3DClass", "Direct Illumination(Phong)", nullptr));
        DirectIndirectPhongCheck->setText(QApplication::translate("V3DClass", "Global Illumination(Phong)", nullptr));
        autoRotate->setText(QApplication::translate("V3DClass", "AutoRotate", nullptr));
    } // retranslateUi

};

namespace Ui {
    class V3DClass: public Ui_V3DClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_V3D_H
