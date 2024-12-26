/**
 * @file
 *
 *  The SNS_RD App header file containing version information
 */

#ifndef SNS_RD_APP_VERSION_H
#define SNS_RD_APP_VERSION_H

/* Development Build Macro Definitions */

#define SNS_RD_APP_BUILD_NUMBER 39 /*!< Development Build: Number of commits since baseline */
#define SNS_RD_APP_BUILD_BASELINE \
    "v1.3.0-rc4" /*!< Development Build: git tag that is the base for the current development */

/*
 * Version Macros, see \ref cfsversions for definitions.
 */
#define SNS_RD_APP_MAJOR_VERSION 1  /*!< @brief Major version number. */
#define SNS_RD_APP_MINOR_VERSION 1  /*!< @brief Minor version number. */
#define SNS_RD_APP_REVISION      99 /*!< @brief Revision version number. Value of 99 indicates a development version.*/

/*!
 * @brief Mission revision.
 *
 * Reserved for mission use to denote patches/customizations as needed.
 * Values 1-254 are reserved for mission use to denote patches/customizations as needed. NOTE: Reserving 0 and 0xFF for
 * cFS open-source development use (pending resolution of nasa/cFS#440)
 */
#define SNS_RD_APP_MISSION_REV 0xFF

#define SNS_RD_APP_STR_HELPER(x) #x /*!< @brief Helper function to concatenate strings from integer macros */
#define SNS_RD_APP_STR(x) \
    SNS_RD_APP_STR_HELPER(x) /*!< @brief Helper function to concatenate strings from integer macros */

/*! @brief Development Build Version Number.
 * @details Baseline git tag + Number of commits since baseline. @n
 * See @ref cfsversions for format differences between development and release versions.
 */
#define SNS_RD_APP_VERSION SNS_RD_APP_BUILD_BASELINE "+dev" SNS_RD_APP_STR(SNS_RD_APP_BUILD_NUMBER)

/*! @brief Development Build Version String.
 * @details Reports the current development build's baseline, number, and name. Also includes a note about the latest
 * official version. @n See @ref cfsversions for format differences between development and release versions.
 */
#define SNS_RD_APP_VERSION_STRING                            \
    " Sensor Read App DEVELOPMENT BUILD " SNS_RD_APP_VERSION \
    ", Last Official Release: v1.1.0" /* For full support please use this version */

#endif /* SNS_RD_APP_VERSION_H */
