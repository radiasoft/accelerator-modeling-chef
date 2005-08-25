/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF:      A Qt-based Application Framework
******             using the BEAMLINE class library.
******                                                                
******  File:      DbRetrieveQueries.h
******                                                                
******  Copyright (c) Universities Research Association, Inc.   
******                All Rights Reserved                             
******                                                                
******  Author:    Jean-Francois Ostiguy
******                                                                
******             Fermilab, Batavia, IL                                
******             ostiguy@fnal.gov                         
******                                                                
******                                                                
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******* U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******* The U.S. Government retains a world-wide non-exclusive, 
******* royalty-free license to publish or reproduce documentation 
******* and software for U.S. Government purposes. This software 
******* is protected under the U.S. and Foreign Copyright Laws. 
******* URA/FNAL reserves all rights.
*******                                                                
**************************************************************************
**************************************************************************
*************************************************************************/
#ifndef DBQUERIES_H
#define DBQUERIES_H

//------------------------------------------------------------------------------
//
// The queries below are meant for the AD BLASTMAN Database Schema. 
// The filter  NOT IN ('0', 'MOVE', 'MSB', 'MTF', 'A-0 NORT', 'A0 HiBay') 
// is a tmp kludge that will (hopefully) go away.
// t_magnet_locations_fixed is a VIEW that fixed bad date data in 
// t_magnet_locations. This should also go away.
// 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// a query that returns a 1-column list of valid (available) machines/beamlines 
//-----------------------------------------------------------------------------

const char valid_machines_query_string[]  = 
"SELECT c_machine_name FROM t_valid_machines ";

//------------------------------------------------------------------------------
// a query that returns a list of ordered locations and azimuths for a given machine
//------------------------------------------------------------------------------

const char machine_locations_query_string[]  = 
"SELECT    t_valid_locations.c_magnet_location AS Location,\n" 
"           0.00 as azimuth_up,\n"  
"           0.00 as azimuth_down\n" 
"FROM      t_valid_locations\n"
"WHERE     t_valid_locations.c_machine_name= ? \n"
"ORDER BY  Location"; 


//------------------------------------------------------------------------------
// a query that returns a list of magnet families for a given machine
//------------------------------------------------------------------------------

const char magnet_series_query_string[]  = 
"SELECT c_magnet_series from t_magnet_series_descriptions\n"
"WHERE  c_machine_name=?";

//------------------------------------------------------------------------------
// a query that returns a list of available survey (XYH) data sets (survey dates)  
//------------------------------------------------------------------------------

const char available_survey_xyh_dates_query_string[]  = 
"SELECT t_survey_xyh.c_calc_date as c_available\n"    
"FROM   t_survey_xyh, \n"
"       (  SELECT  c_magnet_location \n"
"          FROM    t_valid_locations \n"
"          WHERE     (t_valid_locations.c_machine_name=:machine_name) \n"
"          AND (c_magnet_location NOT IN ('0', 'A0 HiBay', 'A-0 NORT', 'A0U-1',\n"
"                                         'MOVE', 'MSB', 'MTF'))\n"
"        ) AS machine_locations\n"
"WHERE\n"
"        (t_survey_xyh.c_magnet_location =  machine_locations.c_magnet_location)\n"
"GROUP BY t_survey_xyh.c_calc_date\n"
"ORDER BY t_survey_xyh.c_calc_date";

//------------------------------------------------------------------------------
// a query that returns a list of available survey (XYZ) data sets (survey dates)  
//------------------------------------------------------------------------------

const char available_survey_xyz_dates_query_string[]  = 
"SELECT t_survey_xyz.c_calc_date as c_available\n"
"FROM   t_survey_xyz,\n"
"       (  SELECT  c_magnet_location\n"
"          FROM    t_valid_locations\n"
"          WHERE     (t_valid_locations.c_machine_name=:machine_name)\n"
"          AND (c_magnet_location NOT IN ('0', 'A0 HiBay', 'A-0 NORT', 'A0U-1',\n"
"                                         'MOVE', 'MSB', 'MTF'))\n"
"        ) AS machine_locations\n"
"WHERE\n"
"        (t_survey_xyz.c_magnet_location =  machine_locations.c_magnet_location)\n"
" "
"GROUP BY t_survey_xyz.c_calc_date\n"
"ORDER BY t_survey_xyz.c_calc_date";


//-------------------------------------------------------------------------------------------
// a query that returns a list of available magnetic moments measurement data sets (by MTF series)  
//-------------------------------------------------------------------------------------------
const char available_magnet_currents_by_series_query_string[]  =  
"SELECT\n" 
"       t_magnetic_moments_idx.c_target_amperage\n"
"FROM t_magnetic_moments_data,"
"     t_magnetic_moments_idx,"
" (  SELECT    t_magnet_locations_fixed.c_magnet_name\n" 
"    FROM      t_magnet_locations_fixed,"
"              t_magnet_names,"
"              t_magnet_types,"
"        ( SELECT     t_valid_locations.c_magnet_location," 
"                                 DATE( ? ) - MIN(  CASE WHEN (DATE( ? ) < DATE(t_magnet_locations_fixed.c_location_date)) THEN NULL\n"
"                                 ELSE (DATE( ? )- DATE(t_magnet_locations_fixed.c_location_date))\n"
"                                 END"
"                              ) AS date\n" 
"          FROM      t_valid_locations," 
"                    t_magnet_locations_fixed\n"  
"          WHERE     ( t_valid_locations.c_machine_name = ? )\n"
"          AND       ( t_valid_locations.c_magnet_location NOT IN ('0', 'MOVE', 'MSB', 'MTF', 'A-0 NORT', 'A0 HiBay'))\n" 
"          AND        ( t_valid_locations.c_magnet_location=t_magnet_locations_fixed.c_magnet_location )\n" 
"          GROUP BY  t_valid_locations.c_magnet_location\n"
"        ) AS configuration_as_of_date\n"
"    WHERE     ( configuration_as_of_date.c_magnet_location=t_magnet_locations_fixed.c_magnet_location )\n" 
"    AND       ( DATE(c_location_date) = configuration_as_of_date.date)\n"
"    AND       ( t_magnet_names.c_magnet_name=t_magnet_locations_fixed.c_magnet_name)\n"
"    AND       ( t_magnet_types.c_magnet_type=t_magnet_names.c_magnet_type)\n" 
"    AND       ( t_magnet_types.c_magnet_series IN ( %%LIST%% ) )\n" 
" ) AS machine_dipoles\n"
"WHERE (t_magnetic_moments_data.c_magnet_name =  machine_dipoles.c_magnet_name)\n"
"AND   (t_magnetic_moments_idx.c_magnet_name  =  machine_dipoles.c_magnet_name)\n"
"AND   (t_magnetic_moments_data.c_step_index  =  t_magnetic_moments_idx.c_step_index)\n"
"AND   (t_magnetic_moments_idx.c_up_down_flag='U')\n"
"GROUP BY t_magnetic_moments_idx.c_target_amperage\n"
"ORDER BY t_magnetic_moments_idx.c_target_amperage";

//-------------------------------------------------------------------------------------------
// a query that returns a list of available magnetic moments measurement data sets (currents)  
//-------------------------------------------------------------------------------------------
const char available_magnet_currents_query_string[]  =  
"SELECT\n" 
"       t_magnetic_moments_idx.c_target_amperage\n"
"FROM t_magnetic_moments_data,"
"     t_magnetic_moments_idx,"
" (  SELECT    t_magnet_locations_fixed.c_magnet_name\n" 
"    FROM      t_magnet_locations_fixed,"
"              t_magnet_names,"
"              t_magnet_types,"
"        ( SELECT     t_valid_locations.c_magnet_location," 
"                                 DATE( ? ) - MIN(  CASE WHEN (DATE( ? ) < DATE(t_magnet_locations_fixed.c_location_date)) THEN NULL\n"
"                                 ELSE (DATE( ? )- DATE(t_magnet_locations_fixed.c_location_date))\n"
"                                 END"
"                              ) AS date\n" 
"          FROM      t_valid_locations," 
"                    t_magnet_locations_fixed\n"  
"          WHERE     ( t_valid_locations.c_machine_name = ? )\n"
"          AND       ( t_valid_locations.c_magnet_location NOT IN ('0', 'MOVE', 'MSB', 'MTF', 'A-0 NORT', 'A0 HiBay'))\n" 
"          AND        ( t_valid_locations.c_magnet_location=t_magnet_locations_fixed.c_magnet_location )\n" 
"          GROUP BY  t_valid_locations.c_magnet_location\n"
"        ) AS configuration_as_of_date\n"
"    WHERE     ( configuration_as_of_date.c_magnet_location=t_magnet_locations_fixed.c_magnet_location )\n" 
"    AND       ( DATE(c_location_date) = configuration_as_of_date.date)\n"
"    AND       ( t_magnet_names.c_magnet_name=t_magnet_locations_fixed.c_magnet_name)\n"
"    AND       ( t_magnet_types.c_magnet_type=t_magnet_names.c_magnet_type)\n" 
"    AND       ( t_magnet_types.c_magnet_numpoles = ?)\n" 
" ) AS machine_dipoles\n"
"WHERE (t_magnetic_moments_data.c_magnet_name =  machine_dipoles.c_magnet_name)\n"
"AND   (t_magnetic_moments_idx.c_magnet_name  =  machine_dipoles.c_magnet_name)\n"
"AND   (t_magnetic_moments_data.c_step_index  =  t_magnetic_moments_idx.c_step_index)\n"
"AND   (t_magnetic_moments_idx.c_up_down_flag='U')\n"
"GROUP BY t_magnetic_moments_idx.c_target_amperage\n"
"ORDER BY t_magnetic_moments_idx.c_target_amperage";

//-------------------------------------------------------------------------------------------
// a query that returns a magnetic moment measurement data sets by current, config date,   
//-------------------------------------------------------------------------------------------

const char magnet_moments_query_string[]  =  
" SELECT  magnet_list.c_magnet_name,"
"         magnet_list.c_magnet_location,"
"         magnet_list.c_location_date,"
"         t_magnetic_moments_data.c_moment_number,"
"         t_magnetic_moments_data.c_b,"
"         t_magnetic_moments_data.c_a,"
"         t_magnetic_moments_idx.c_target_amperage,"
"         t_magnetic_moments_idx.c_step_index,"
"         t_magnet_names.c_magnet_type\n"
" FROM\n"
"         t_magnetic_moments_data,"
"         t_magnetic_moments_idx,"
"         t_magnet_names,"
"         t_magnet_types,"
"         ( SELECT   t_magnet_locations_fixed.c_magnet_name,"
"                    t_magnet_locations_fixed.c_magnet_location,"
"                    t_magnet_locations_fixed.c_location_date\n"
"            FROM    t_magnet_locations_fixed,"
"                    (   SELECT     t_valid_locations.c_magnet_location,"
"                           ( DATE( ? ) - MIN(\n"
"                              CASE\n"
"                                WHEN (DATE( ? ) < DATE(t_magnet_locations_fixed.c_location_date)) THEN NULL\n"
"                             ELSE\n"
"                                (DATE( ? )- DATE(t_magnet_locations_fixed.c_location_date))\n"
"                             END )"
"                            ) AS date\n"
"                     FROM      t_valid_locations,"
"                               t_magnet_locations_fixed\n"
"                     WHERE     ( t_valid_locations.c_machine_name= ? )\n"
"                     AND       ( t_valid_locations.c_magnet_location NOT IN ('0', 'MOVE', 'MSB', 'MTF', 'A-0 NORT', 'A0 HiBay'))\n"
"                     AND       ( t_valid_locations.c_magnet_location=t_magnet_locations_fixed.c_magnet_location )\n"
"                     GROUP BY  t_valid_locations.c_magnet_location"
"                     )   AS machine_state\n"
"            WHERE  (t_magnet_locations_fixed.c_magnet_location       =  machine_state.c_magnet_location)\n"
"            AND    (DATE(t_magnet_locations_fixed.c_location_date)   =  machine_state.date )\n"
"          ) as magnet_list\n"
"WHERE     (t_magnetic_moments_data.c_magnet_name     =  magnet_list.c_magnet_name)\n"
"AND       (t_magnetic_moments_idx.c_magnet_name      =  magnet_list.c_magnet_name)\n"
"AND       (t_magnetic_moments_idx.c_step_index       =  t_magnetic_moments_data.c_step_index)\n"
"AND       (t_magnetic_moments_idx.c_up_down_flag     =  'U' )\n"
"AND       (t_magnetic_moments_idx.c_target_amperage  = ? )\n"
"AND       (t_magnet_names.c_magnet_name              = magnet_list.c_magnet_name )\n"
"AND       (t_magnet_types.c_magnet_type              = t_magnet_names.c_magnet_type )\n"
"AND       (t_magnet_types.c_magnet_numpoles          = ? )\n"
"ORDER BY  magnet_list.c_magnet_location, t_magnetic_moments_data.c_moment_number ";

//---------------------------------------------------------------------------------------------------
// a query that returns a magnetic moment measurement data sets by current, config date, magnet series  
//----------------------------------------------------------------------------------------------------

const char magnet_moments_by_series_query_string[]  =  
" SELECT  magnet_list.c_magnet_name,"
"         magnet_list.c_magnet_location,"
"         magnet_list.c_location_date,"
"         t_magnetic_moments_data.c_moment_number,"
"         t_magnetic_moments_data.c_b,"
"         t_magnetic_moments_data.c_a,"
"         t_magnetic_moments_idx.c_target_amperage,"
"         t_magnetic_moments_idx.c_step_index,"
"         t_magnet_names.c_magnet_type\n"
" FROM\n"
"         t_magnetic_moments_data,"
"         t_magnetic_moments_idx,"
"         t_magnet_names,"
"         t_magnet_types,"
"         ( SELECT   t_magnet_locations_fixed.c_magnet_name,"
"                    t_magnet_locations_fixed.c_magnet_location,"
"                    t_magnet_locations_fixed.c_location_date\n"
"            FROM    t_magnet_locations_fixed,"
"                    (   SELECT     t_valid_locations.c_magnet_location,"
"                           ( DATE( ? ) - MIN(\n"
"                              CASE\n"
"                                WHEN (DATE( ? ) < DATE(t_magnet_locations_fixed.c_location_date)) THEN NULL\n"
"                             ELSE\n"
"                                (DATE( ? )- DATE(t_magnet_locations_fixed.c_location_date))\n"
"                             END )"
"                            ) AS date\n"
"                     FROM      t_valid_locations,"
"                               t_magnet_locations_fixed\n"
"                     WHERE     ( t_valid_locations.c_machine_name= ? )\n"
"                     AND       ( t_valid_locations.c_magnet_location NOT IN ('0', 'MOVE', 'MSB', 'MTF', 'A-0 NORT', 'A0 HiBay'))\n"
"                     AND       ( t_valid_locations.c_magnet_location=t_magnet_locations_fixed.c_magnet_location )\n"
"                     GROUP BY  t_valid_locations.c_magnet_location"
"                     )   AS machine_state\n"
"            WHERE  (t_magnet_locations_fixed.c_magnet_location       =  machine_state.c_magnet_location)\n"
"            AND    (DATE(t_magnet_locations_fixed.c_location_date)   =  machine_state.date )\n"
"          ) as magnet_list\n"
"WHERE     (t_magnetic_moments_data.c_magnet_name     =  magnet_list.c_magnet_name)\n"
"AND       (t_magnetic_moments_idx.c_magnet_name      =  magnet_list.c_magnet_name)\n"
"AND       (t_magnetic_moments_idx.c_step_index       =  t_magnetic_moments_data.c_step_index)\n"
"AND       (t_magnetic_moments_idx.c_up_down_flag     =  'U' )\n"
"AND       (t_magnetic_moments_idx.c_target_amperage  = ? )\n"
"AND       (t_magnet_names.c_magnet_name              = magnet_list.c_magnet_name )\n"
"AND       (t_magnet_types.c_magnet_type              = t_magnet_names.c_magnet_type )\n"
"AND       (t_magnet_types.c_magnet_series              IN ( %%LIST%% ) )\n"
"ORDER BY  magnet_list.c_magnet_location, t_magnetic_moments_data.c_moment_number ";


//-------------------------------------------------------------------------------------------
// a query that returns survey XYH data sets by date 
//-------------------------------------------------------------------------------------------
const char* survey_xyh_data_query_string =
"SELECT  t_survey_xyh.* \n" 
"FROM\n"
"    t_survey_xyh,"
"(   SELECT  t_survey_xyh.c_magnet_location," 
"               ( DATE( ? )  -  MIN(   CASE WHEN (DATE( ? ) < DATE(t_survey_xyh.c_calc_date)) THEN NULL\n"
"                                            ELSE (DATE( ? )- DATE(t_survey_xyh.c_calc_date))\n"
"                                            END\n"
"                                        )"
"                ) AS date\n" 
"    FROM    t_survey_xyh, \n"
"            (  SELECT  c_magnet_location\n"
"               FROM    t_valid_locations\n"
"               WHERE     (t_valid_locations.c_machine_name= ? )\n"
"               AND (c_magnet_location NOT IN ('0', 'A0 HiBay', 'A-0 NORT', 'A0U-1',"
"                                              'MOVE', 'MSB', 'MTF'))\n"
"             ) AS machine_locations\n"
"     WHERE     t_survey_xyh.c_magnet_location =  machine_locations.c_magnet_location\n"
"     GROUP BY  t_survey_xyh.c_magnet_location \n"
") AS  survey_data_set \n"
"WHERE \n"
"     ( survey_data_set.c_magnet_location = t_survey_xyh.c_magnet_location)\n"
"AND  ( survey_data_set.date       = DATE(t_survey_xyh.c_calc_date))\n"
"ORDER BY survey_data_set.c_magnet_location";

//-------------------------------------------------------------------------------------------
// a query that returns survey XYZ data sets by date 
//-------------------------------------------------------------------------------------------

const char* survey_xyz_data_query_string =
"SELECT  t_survey_xyz.* \n" 
"FROM\n"
"    t_survey_xyz,"
"(   SELECT  t_survey_xyz.c_magnet_location," 
"               ( DATE( ? )  -  MIN(   CASE WHEN (DATE( ?) < DATE(t_survey_xyz.c_calc_date)) THEN NULL\n"
"                                            ELSE (DATE( ? )- DATE(t_survey_xyz.c_calc_date))\n"
"                                            END\n"
"                                        )"
"                ) AS date\n" 
"    FROM    t_survey_xyz, \n"
"            (  SELECT  c_magnet_location\n"
"               FROM    t_valid_locations\n"
"               WHERE     (t_valid_locations.c_machine_name= ?)\n"
"               AND (c_magnet_location NOT IN ('0', 'A0 HiBay', 'A-0 NORT', 'A0U-1',"
"                                              'MOVE', 'MSB', 'MTF'))\n"
"             ) AS machine_locations\n"
"     WHERE     t_survey_xyz.c_magnet_location =  machine_locations.c_magnet_location\n"
"     GROUP BY  t_survey_xyz.c_magnet_location \n"
") AS  survey_data_set \n"
"WHERE \n"
"     ( survey_data_set.c_magnet_location = t_survey_xyz.c_magnet_location)\n"
"AND  ( survey_data_set.date       = DATE(t_survey_xyz.c_calc_date))\n"
"ORDER BY survey_data_set.c_magnet_location";

//-------------------------------------------------------------------------------------------
// A query that returns survey XYH data sets by date and magnet series
//
// NB: %%LIST%% is a string meant to be replaced by the calling program
//-------------------------------------------------------------------------------------------

const char* survey_xyh_data_by_series_query_string =
"SELECT  t_survey_xyh.* \n" 
"FROM\n"
"    t_magnet_types,\n"
"    t_magnet_names,\n"
"    t_survey_xyh,"
"(   SELECT  t_survey_xyh.c_magnet_location," 
"               ( DATE( ? )  -  MIN(   CASE WHEN (DATE( ? ) < DATE(t_survey_xyh.c_calc_date)) THEN NULL\n"
"                                            ELSE (DATE( ? )- DATE(t_survey_xyh.c_calc_date))\n"
"                                            END\n"
"                                        )"
"                ) AS date\n" 
"    FROM    t_survey_xyh, \n"
"            (  SELECT  c_magnet_location\n"
"               FROM    t_valid_locations\n"
"               WHERE     (t_valid_locations.c_machine_name= ? )\n"
"               AND (c_magnet_location NOT IN ('0', 'A0 HiBay', 'A-0 NORT', 'A0U-1',"
"                                              'MOVE', 'MSB', 'MTF'))\n"
"             ) AS machine_locations\n"
"     WHERE     t_survey_xyh.c_magnet_location =  machine_locations.c_magnet_location\n"
"     GROUP BY  t_survey_xyh.c_magnet_location \n"
") AS  survey_data_set \n"
"WHERE \n"
"     ( survey_data_set.c_magnet_location = t_survey_xyh.c_magnet_location)\n"
"AND  ( survey_data_set.date         = DATE(t_survey_xyh.c_calc_date))\n"
"AND  ( t_magnet_names.c_magnet_name =  t_survey_xyh.c_magnet_name )\n"
"AND  ( t_magnet_names.c_magnet_type =  t_magnet_types.c_magnet_type)\n"
"AND  ( t_magnet_types.c_magnet_series IN ( %%LIST%%) )\n"
"ORDER BY survey_data_set.c_magnet_location";

//-------------------------------------------------------------------------------------------
// A query that returns survey XYZ data sets by date and magnet series
//
// NB: %%LIST%% is a string meant to be replaced by the calling program
//-------------------------------------------------------------------------------------------

const char* survey_xyz_data_by_series_query_string =
"SELECT  t_survey_xyz.* \n" 
"FROM\n"
"    t_magnet_types,\n"
"    t_magnet_names,\n"
"    t_survey_xyz,\n"
"(   SELECT  t_survey_xyz.c_magnet_location," 
"               ( DATE( ? )  -  MIN(   CASE WHEN (DATE( ?) < DATE(t_survey_xyz.c_calc_date)) THEN NULL\n"
"                                            ELSE (DATE( ? )- DATE(t_survey_xyz.c_calc_date))\n"
"                                            END\n"
"                                        )"
"                ) AS date\n" 
"    FROM    t_survey_xyz, \n"
"            (  SELECT  c_magnet_location\n"
"               FROM    t_valid_locations\n"
"               WHERE     (t_valid_locations.c_machine_name= ?)\n"
"               AND (c_magnet_location NOT IN ('0', 'A0 HiBay', 'A-0 NORT', 'A0U-1',"
"                                              'MOVE', 'MSB', 'MTF'))\n"
"             ) AS machine_locations\n"
"     WHERE     t_survey_xyz.c_magnet_location =  machine_locations.c_magnet_location\n"
"     GROUP BY  t_survey_xyz.c_magnet_location\n"
") AS  survey_data_set \n"
"WHERE \n"
"     ( survey_data_set.c_magnet_location = t_survey_xyz.c_magnet_location)\n"
"AND  ( survey_data_set.date       = DATE(t_survey_xyz.c_calc_date))\n"
"AND  ( t_magnet_names.c_magnet_name =  t_survey_xyz.c_magnet_name )\n"
"AND  ( t_magnet_names.c_magnet_type =  t_magnet_types.c_magnet_type)\n"
"AND  ( t_magnet_types.c_magnet_series IN ( %%LIST%%) )\n"
"ORDER BY survey_data_set.c_magnet_location";

#endif
