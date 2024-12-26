
/**
 * @file
 *
 *  The SNS_RD App header file containing version information
 */

#ifndef UDP_TLM_VERSION_H
#define UDP_TLM_VERSION_H

/* Development Build Macro Definitions */

#define UDP_TLM_BUILD_NUMBER 39 /*!< Development Build: Number of commits since baseline */
#define UDP_TLM_BUILD_BASELINE \
    "v1.3.0-rc4" /*!< Development Build: git tag that is the base for the current development */

/*
 * Version Macros, see \ref cfsversions for definitions.
 */
#define UDP_TLM_MAJOR_VERSION 1  /*!< @brief Major version number. */
#define UDP_TLM_MINOR_VERSION 1  /*!< @brief Minor version number. */
#define UDP_TLM_REVISION      99 /*!< @brief Revision version number. Value of 99 indicates a development version.*/

/*!
 * @brief Mission revision.
 *
 * Reserved for mission use to denote patches/customizations as needed.
 * Values 1-254 are reserved for mission use to denote patches/customizations as needed. NOTE: Reserving 0 and 0xFF for
 * cFS open-source development use (pending resolution of nasa/cFS#440)
 */
#define UDP_TLM_MISSION_REV 0xFF

#define UDP_TLM_STR_HELPER(x) #x /*!< @brief Helper function to concatenate strings from integer macros */
#define UDP_TLM_STR(x)        UDP_TLM_STR_HELPER(x) /*!< @brief Helper function to concatenate strings from integer macros */

/*! @brief Development Build Version Number.
 * @details Baseline git tag + Number of commits since baseline. @n
 * See @ref cfsversions for format differences between development and release versions.
 */
#define UDP_TLM_VERSION UDP_TLM_BUILD_BASELINE "+dev" UDP_TLM_STR(UDP_TLM_BUILD_NUMBER)

/*! @brief Development Build Version String.
 * @details Reports the current development build's baseline, number, and name. Also includes a note about the latest
 * official version. @n See @ref cfsversions for format differences between development and release versions.
 */
#define UDP_TLM_VERSION_STRING                              \
    " UDP Telemetry App DEVELOPMENT BUILD " UDP_TLM_VERSION \
    ", Last Official Release: v1.1.0" /* For full support please use this version */

#endif /* UDP_TLM_VERSION_H */
