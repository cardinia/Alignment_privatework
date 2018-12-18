# Alignment_privatework
Repository for my private work regarding Alignment in the CMS experiment

Instructions for running the scripts and plotting macros:

Work in progress regarding the DMR trend tool:

To run the DMR trends tool it is necessary to first run the Offline Validation using the all-in-one tool as per usual.
However when running the TkAlMerge.sh script it is required to use the code currently stored in the following area:
/afs/cern.ch/work/a/acardini/Alignment/MultiIOV/CMSSW_10_4_0_pre4/src
This passage will be removed once a PR is done and the all-in-one tool is updated with the feature I implemented to save the histograms of the DMRs in a summary root file.

Once this passage is done the tool can be run:
1) Edit the main() function in DMRtrends.C macro [1] to select the directory where the DMR summary files for the different IOVs are stored and state the labels for the geometries
2) Compile the DMRtrends.C macro with make (a Makefile is available in this repository)
3) Execute with ./DMRtrends

[1]https://github.com/cardinia/Alignment_privatework/blob/master/DMRtrends.C#L490


DEPRECATED method:

Premises:
- the script printsummaryFromDMRs.sh (and the corresponding script for Single Muon, printsummaryFromDMRs_SM.sh) are used to produce text files containing the run number and the values for the various variables related to the DMRs: the average, the deviation from the average and dmu;
- the corresponding plotting macro is DMu_plotter.C

- for plotting the trend of the averages using their appropriate uncertainties the scripts printsummaryFromDMRs_musigma.sh and printsummaryFromDMRs_musigma_SM.sh are used, while the corresponding plots are made with MuSigma_plotter.C


Before running:
- edit the names of the geometries in both printsummaryFromDMRs.sh and printsummaryFromDMRs_musigma.sh
- edit the path to the directory where the DMRs are stored
- edit the label using the series of characters wrote at the end of the name of the directory for the DMR refered to a specific IOV (the one currently stored is "_v9")
- eventually edit the output directory (in this case also the path in the plotting macros needs to be changed accordingly)

To run the scripts on Minimum Bias and Single Muon at the same time I personally prefer to copy printsummaryFromDMRs.sh and printsummaryFromDMRs_musigma.sh and make identical scripts in which I change the MB string to SM.


NOTE:

All the scripts require the geometries to be ordered in the same way as they appear using the command "printsummarytable.py" in the directories where the DMRs are stored. It is not currently possible to run on a subset of the geometries.
In the plotting macros however the geometries that are actually plotted can also be a subset of all the ones available.


As for the plotting macros the changes to do before running are the following:
- edit the names of the geometries so that they match the ones used by the previous scripts
- edit the plotting colors accordingly
