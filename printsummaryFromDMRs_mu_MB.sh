#!/bin/zsh
DMRdir=/afs/cern.ch/cms/CAF/CMSALCA/ALCA_TRACKERALIGN/data/commonValidation/results/paconnor
printdir=/afs/cern.ch/cms/CAF/CMSALCA/ALCA_TRACKERALIGN/data/commonValidation/alignmentObjects/acardini/dMuDMR_MB
#cd $DMRdir
cd $printdir
geometries=(GT SG MPpixLBL MPpixLBLstr MP2pixLBLHGIOV MP2pixHMSHGIOV MP2pixHMSstrHGIOV HipPypix)
typ=MB
input=mu
variables=(mu dmu sigma)
structures=(BPIX FPIX BPIX_y FPIX_y TIB TOB TID TEC)

makefilename () {
    structure=$1
    geometry=$2
    variable=$3
    echo $printdir/${variable}_${typ}_${structure}_${geometry}.dat
}
for structure in "${structures[@]}"
do
    for geometry in "${geometries[@]}"
    do
	for variable in "${variables[@]}"
	do
            filename=`makefilename $structure $geometry $variable`
            echo $filename
            rm -f $filename
            touch $filename
	done
    done
done

runs=(313041 313802 314675 314881 315257 315488 315489 315506 315640 315689 315690 315713 315790 315800 315973 316058 316060 316082 316187 316199 316200 316216 316218 316239 316271 316361 316363 316378 316456 316470 316505 316559 316569 316665 316758 317080 317182 317212 317295 317339 317382 317438 317527 317661 318228 318954 318712 319337 319460 320377 320569 320688 320712 320809 320821 320823 320838 320841 320853 320856 320877 320917 320933 320936 320980 321004 321009 321051 321067 321069 321119 321149 321164 321219 321221 321282)

for run in "${runs[@]}"
do
    dir=$DMRdir/DMR_${typ}_${run}_v4/ExtendedOfflineValidation_Images/
    filetest=DmedianR_BPIX.pdf
    echo $dir
    if [ -f "$dir/$filetest" ]
    then
        cd $dir
	for structure in "${structures[@]}"
	do
            file=`printsummarytable.py`
	    line=`echo $file | grep "${input}_${structure} (um)"`
            i=3
	    for geometry in "${geometries[@]}"
	    do
		value1=`echo $line | awk -v a=$i {'print $a'} | sed -n 1p`
		variable1=`echo $line | awk {'print $1'} | sed -n 1p`
		filename1=`makefilename $structure $geometry $variable1`
		echo "$run $structure $geometry $value $variable1"
		echo "$run $value" >> $filename1
		value2=`echo $line | awk -v a=$i {'print $a'} | sed -n 2p`
		variable2=`echo $line | awk {'print $1'} | sed -n 2p`
		filename2=`makefilename $structure $geometry $variable2`
		echo "$run $structure $geometry $value $variable2"
		echo "$run $value" >> $filename2
		value3=`echo $line | awk -v a=$i {'print $a'} | sed -n 3p`
		variable3=`echo $line | awk {'print $1'} | sed -n 3p`
		filename3=`makefilename $structure $geometry $variable3`
		echo "$run $structure $geometry $value $variable3"
		echo "$run $value" >> $filename3
	        i=`echo "$i+1" | bc`
	    done
        done
	cd -
    else
	echo "empty or non existing"
    fi
done
