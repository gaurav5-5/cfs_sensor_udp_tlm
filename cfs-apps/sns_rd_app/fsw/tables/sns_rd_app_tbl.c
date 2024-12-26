#include "cfe_tbl_filedef.h" /* Required to obtain the CFE_TBL_FILEDEF macro definition */
#include "sns_rd_app_table.h"

/*
** The following is an example of the declaration statement that defines the desired
** contents of the table image.
*/
SNS_RD_APP_Table_t SnsDataTable = {500U, 100U};

/*
** The macro below identifies:
**    1) the data structure type to use as the table image format
**    2) the name of the table to be placed into the cFE Table File Header
**    3) a brief description of the contents of the file image
**    4) the desired name of the table image binary file that is cFE compatible
*/
CFE_TBL_FILEDEF(SnsDataTable, SNS_RD_APP.SnsDataTable, Sensor Configuration Table, sns_rd_app_tbl.tbl)
