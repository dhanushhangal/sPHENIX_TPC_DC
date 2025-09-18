# analyze_dc_trees — Digital Current (DC) tree analysis

This folder contains quick-turnaround ROOT macros and tiny utilities I use to inspect **DigitalCurrentDebugTTree** outputs from sPHENIX streaming data (“DC trees”).

> - Point the macro run_digital_current.C at a list of `.root` files (DC trees), and this runs the T_Digital_Current.C macro over the 24 ebdc DC files to create two TH2D histograms.
> - The two TH2D histograms are (a) channel vs. gtm_bco (weighted by adc) and (b) channel vs. frame_index (weighted by adc) (created by default for the first 80 frame indices)
> - Running the fee_plot_tpcvolume macro with the (b) channel vs. frame_index (weighted by adc) histogram as inputs for 24 sectors produces a TH3D of DC over the entire TPC volume for 80 frame indices.
> - You can instead run it over individual frame index by specify the ProjectionX bins in L197 of fee_plot_tpcvolume.C
