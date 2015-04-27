//-------------------------------------------------------------------------//
//          Windows Graphics Programming: Win32 GDI and DirectDraw         //
//                        ISBN  0-13-086985-6                              //
//                                                                         //
//  Modified by: Yuan, Feng                             www.fengyuan.com   //
//  Changes    : C++, exception, in-memory source, BGR byte order          //
//  Version    : 1.00.000, May 31, 2000                                    //
//-------------------------------------------------------------------------//

/*
 * jdapistd.c
 *
 * Copyright (C) 1994-1996, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains application interface code for the decompression half
 * of the JPEG library.  These are the "standard" API routines that are
 * used in the normal full-decompression case.  They are not used by a
 * transcoding-only application.  Note that if an application links in
 * jpeg_start_decompress, it will end up linking in the entire decompressor.
 * We thus must separate this file from jdapimin.c to avoid linking the
 * whole decompression library into a transcoder.
 */

#define JPEG_INTERNALS
#include "jinclude.h"
#include "jpeglib.h"


/* Forward declarations */
LOCAL(boolean) output_pass_setup (j_decompress_ptr cinfo);


/*
 * Decompression initialization.
 * jpeg_read_header must be completed before calling this.
 *
 * If a multipass operating mode was selected, this will do all but the
 * last pass, and thus may take a great deal of time.
 *
 * Returns FALSE if suspended.  The return value need be inspected only if
 * a suspending data source is used.
 */

boolean jpeg_decompress_struct::jpeg_start_decompress(void)
{
	if (global_state == DSTATE_READY) 
	{
		/* First call: initialize master control, select active modules */
		jinit_master_decompress(this);
		
		if (buffered_image) 
		{
			/* No more work here; expecting jpeg_start_output next */
			global_state = DSTATE_BUFIMAGE;
			return TRUE;
		}
		global_state = DSTATE_PRELOAD;
	}
  
	if (global_state == DSTATE_PRELOAD) 
	{
		/* If file has multiple scans, absorb them all into the coef buffer */
		if (inputctl->has_multiple_scans) 
		{
#ifdef D_MULTISCAN_FILES_SUPPORTED
			for (;;) 
			{
				int retcode;
				/* Call progress monitor hook if present */
				if (progress != NULL)
					progress->progress_monitor(this);
	
				/* Absorb some more input */
				retcode = (*inputctl->consume_input) (this);
				
				if (retcode == JPEG_SUSPENDED)
					return FALSE;
				
				if (retcode == JPEG_REACHED_EOI)
					break;
	
				/* Advance progress counter if appropriate */
				if (progress != NULL &&
					(retcode == JPEG_ROW_COMPLETED || retcode == JPEG_REACHED_SOS)) 
				{
					if (++progress->pass_counter >= progress->pass_limit) 
					{
						/* jdmaster underestimated number of scans; ratchet up one scan */
						progress->pass_limit += (long) total_iMCU_rows;
					}
				}
			}
#else
			ERREXIT(this, JERR_NOT_COMPILED);
#endif /* D_MULTISCAN_FILES_SUPPORTED */
		}
		
		output_scan_number = input_scan_number;
	} 
	else if (global_state != DSTATE_PRESCAN)
		ERREXIT1(JERR_BAD_STATE, global_state);
  
	/* Perform any dummy output passes, and set up for the final pass */
	return output_pass_setup(this);
}


/*
 * Set up for an output pass, and perform any dummy pass(es) needed.
 * Common subroutine for jpeg_start_decompress and jpeg_start_output.
 * Entry: global_state = DSTATE_PRESCAN only if previously suspended.
 * Exit: If done, returns TRUE and sets global_state for proper output mode.
 *       If suspended, returns FALSE and sets global_state = DSTATE_PRESCAN.
 */

LOCAL(boolean)
output_pass_setup (j_decompress_ptr cinfo)
{
  if (cinfo->global_state != DSTATE_PRESCAN) {
    /* First call: do pass setup */
    (*cinfo->master->prepare_for_output_pass) (cinfo);
    cinfo->output_scanline = 0;
    cinfo->global_state = DSTATE_PRESCAN;
  }
  /* Loop over any required dummy passes */
  while (cinfo->master->is_dummy_pass) {
#ifdef QUANT_2PASS_SUPPORTED
    /* Crank through the dummy pass */
    while (cinfo->output_scanline < cinfo->output_height) {
      JDIMENSION last_scanline;
      /* Call progress monitor hook if present */
      if (cinfo->progress != NULL) {
	cinfo->progress->pass_counter = (long) cinfo->output_scanline;
	cinfo->progress->pass_limit = (long) cinfo->output_height;
	cinfo->progress->progress_monitor(cinfo);
      }
      /* Process some data */
      last_scanline = cinfo->output_scanline;
      (*cinfo->main->process_data) (cinfo, (JSAMPARRAY) NULL,
				    &cinfo->output_scanline, (JDIMENSION) 0);
      if (cinfo->output_scanline == last_scanline)
	return FALSE;		/* No progress made, must suspend */
    }
    /* Finish up dummy pass, and set up for another one */
    (*cinfo->master->finish_output_pass) (cinfo);
    (*cinfo->master->prepare_for_output_pass) (cinfo);
    cinfo->output_scanline = 0;
#else
    ERREXIT(cinfo, JERR_NOT_COMPILED);
#endif /* QUANT_2PASS_SUPPORTED */
  }
  /* Ready for application to drive output pass through
   * jpeg_read_scanlines or jpeg_read_raw_data.
   */
  cinfo->global_state = cinfo->raw_data_out ? DSTATE_RAW_OK : DSTATE_SCANNING;
  return TRUE;
}


/*
 * Read some scanlines of data from the JPEG decompressor.
 *
 * The return value will be the number of lines actually read.
 * This may be less than the number requested in several cases,
 * including bottom of image, data source suspension, and operating
 * modes that emit multiple scanlines at a time.
 *
 * Note: we warn about excess calls to jpeg_read_scanlines() since
 * this likely signals an application programmer error.  However,
 * an oversize buffer (max_lines > scanlines remaining) is not an error.
 */

JDIMENSION jpeg_decompress_struct::jpeg_read_scanlines(JSAMPARRAY scanlines, JDIMENSION max_lines)
{
	JDIMENSION row_ctr;

	if ( global_state != DSTATE_SCANNING )
		ERREXIT1(JERR_BAD_STATE, global_state);
  
	if ( output_scanline >= output_height) 
	{
		WARNMS(JWRN_TOO_MUCH_DATA);
		return 0;
	}

	/* Call progress monitor hook if present */
	if (progress != NULL) 
	{
		progress->pass_counter = (long) output_scanline;
		progress->pass_limit = (long) output_height;
		progress->progress_monitor(this);
	}

	/* Process some data */
	row_ctr = 0;
	(*main->process_data) (this, scanlines, &row_ctr, max_lines);
	output_scanline += row_ctr;
	
	return row_ctr;
}


/*
 * Alternate entry point to read raw data.
 * Processes exactly one iMCU row per call, unless suspended.
 */

GLOBAL(JDIMENSION)
jpeg_read_raw_data (j_decompress_ptr cinfo, JSAMPIMAGE data,
		    JDIMENSION max_lines)
{
  JDIMENSION lines_per_iMCU_row;

  if (cinfo->global_state != DSTATE_RAW_OK)
    cinfo->ERREXIT1(JERR_BAD_STATE, cinfo->global_state);
  if (cinfo->output_scanline >= cinfo->output_height) {
    cinfo->WARNMS(JWRN_TOO_MUCH_DATA);
    return 0;
  }

  /* Call progress monitor hook if present */
  if (cinfo->progress != NULL) {
    cinfo->progress->pass_counter = (long) cinfo->output_scanline;
    cinfo->progress->pass_limit = (long) cinfo->output_height;
    cinfo->progress->progress_monitor(cinfo);
  }

  /* Verify that at least one iMCU row can be returned. */
  lines_per_iMCU_row = cinfo->max_v_samp_factor * cinfo->min_DCT_scaled_size;
  if (max_lines < lines_per_iMCU_row)
    cinfo->ERREXIT(JERR_BUFFER_SIZE);

  /* Decompress directly into user's buffer. */
  if (! (*cinfo->coef->decompress_data) (cinfo, data))
    return 0;			/* suspension forced, can do nothing more */

  /* OK, we processed one iMCU row. */
  cinfo->output_scanline += lines_per_iMCU_row;
  return lines_per_iMCU_row;
}


/* Additional entry points for buffered-image mode. */

#ifdef D_MULTISCAN_FILES_SUPPORTED

/*
 * Initialize for an output pass in buffered-image mode.
 */

GLOBAL(boolean)
jpeg_start_output (j_decompress_ptr cinfo, int scan_number)
{
  if (cinfo->global_state != DSTATE_BUFIMAGE &&
      cinfo->global_state != DSTATE_PRESCAN)
    cinfo->ERREXIT1(JERR_BAD_STATE, cinfo->global_state);
  /* Limit scan number to valid range */
  if (scan_number <= 0)
    scan_number = 1;
  if (cinfo->inputctl->eoi_reached &&
      scan_number > cinfo->input_scan_number)
    scan_number = cinfo->input_scan_number;
  cinfo->output_scan_number = scan_number;
  /* Perform any dummy output passes, and set up for the real pass */
  return output_pass_setup(cinfo);
}


/*
 * Finish up after an output pass in buffered-image mode.
 *
 * Returns FALSE if suspended.  The return value need be inspected only if
 * a suspending data source is used.
 */

GLOBAL(boolean)
jpeg_finish_output (j_decompress_ptr cinfo)
{
  if ((cinfo->global_state == DSTATE_SCANNING ||
       cinfo->global_state == DSTATE_RAW_OK) && cinfo->buffered_image) {
    /* Terminate this pass. */
    /* We do not require the whole pass to have been completed. */
    (*cinfo->master->finish_output_pass) (cinfo);
    cinfo->global_state = DSTATE_BUFPOST;
  } else if (cinfo->global_state != DSTATE_BUFPOST) {
    /* BUFPOST = repeat call after a suspension, anything else is error */
    cinfo->ERREXIT1(JERR_BAD_STATE, cinfo->global_state);
  }
  /* Read markers looking for SOS or EOI */
  while (cinfo->input_scan_number <= cinfo->output_scan_number &&
	 ! cinfo->inputctl->eoi_reached) {
    if ((*cinfo->inputctl->consume_input) (cinfo) == JPEG_SUSPENDED)
      return FALSE;		/* Suspend, come back later */
  }
  cinfo->global_state = DSTATE_BUFIMAGE;
  return TRUE;
}

#endif /* D_MULTISCAN_FILES_SUPPORTED */
