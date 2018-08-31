#!/bin/zsh
#DMRdir=/afs/cern.ch/cms/CAF/CMSALCA/ALCA_TRACKERALIGN/data/commonValidation/results/paconnor
DMRdir=/afs/cern.ch/cms/CAF/CMSALCA/ALCA_TRACKERALIGN/data/commonValidation/results/hpeterse
printdir=/afs/cern.ch/cms/CAF/CMSALCA/ALCA_TRACKERALIGN/data/commonValidation/alignmentObjects/acardini/MuDMR_MB
#cd $DMRdir
cd $printdir
geometries=(GT SG weight10xZmumu+cosmics weight5xZmumu+cosmics weight20xZmumu+cosmics)
typ=MB
#variables=(" mu" "dmu" "sigma" "sigma_mu")
#structures=(BPIX FPIX BPIX_y FPIX_y TIB TOB TID TEC)
structures=(BPIX FPIX BPIX_y FPIX_y TIB TOB)

makefilename () {
    structure=$1
    geometry=$2
    echo $printdir/muandsigma_${typ}_${structure}_${geometry}.dat | tr -d '[:space:]'
}
for structure in "${structures[@]}"
do
    for geometry in "${geometries[@]}"
    do
        filename=`makefilename $structure $geometry`
        echo $filename
        rm -f $filename
        touch $filename
    done
done

runs=(313041 313802 314675 314881 315257 315488 315489 315506 315640 315689 315690 315713 315790 315800 315973 316058 316060 316082 316187 316199 316200 316216 316218 316239 316271 316361 316363 316378 316456 316470 316505 316559 316569 316665 316758 317080 317182 317212 317295 317339 317382 317438 317527 317661 318228 318954 318712 319337 319460 320377 320569 320688 320712 320809 320821 320823 320838 320841 320853 320856 320877 320917 320933 320936 320980 321004 321009 321051 321067 321069 321119 321149 321164 321219 321221 321282)

for run in "${runs[@]}"
do
    dir=$DMRdir/DMR_${typ}_${run}_v9/ExtendedOfflineValidation_Images/
    filetest=DmedianR_BPIX.pdf
    echo $dir
    if [ -f "$dir/$filetest" ]
    then
        cd $dir
	for structure in "${structures[@]}"
	do
	    file=`printsummarytable.py`
	    mu=`echo $file | grep " mu_${structure} (um)"`
	    sigma=`echo $file | grep "sigma_mu_${structure} (um)"`
	    i=3
	    echo "$run $structure"
	    
	    for geometry in "${geometries[@]}"
	    do
		muvalue=`echo $mu | awk -v a=$i {'print $a'}`
		sigmavalue=`echo $sigma | awk -v a=$i {'print $a'}`
		filename=`makefilename $structure $geometry`
		echo "$run $muvalue $sigmavalue" >> $filename
	        i=`echo "$i+1" | bc`
	    done
	done
	cd -
    else
	echo "empty or non existing"
    fi
done
