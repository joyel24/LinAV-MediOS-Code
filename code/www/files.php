<?
	require("include/enteteHtml.php");
	require("include/affBlock.class");
	require("include/dispItem.class");

	require("include/affPage.class");
	require("include/configTable.inc");

	entete();
	//$titre2="FILES";
	//$page=new affPage($titre,$titre2);


	function addFile($url,$name,$txt)
	{
	
		global $doc;
		if($url!="")
			$doc->addItem("<a href='$url' target='_blank'>$name</a>",$txt);
		else
			$doc->addItem("",$txt);
	}

?>
	<table width='100%'>
	<tr>
	<td>
<?

	$doc=new dispItem("Doc");
/////////////////////////////////////////////////////////////////
	addFile("","","<font color='red'><b>uClinux sources used in this project</b></font>");
	addFile("","","<font color='blue'><b>Some of the source files for uClinux need to be patched to work 
	with the AV300. You can download the patch below. You must extract the contents of the TAR file to the
	 same path your uClinux directory is in so that the patched files will overwrite the old ones.</b></font>");
	
	addFile("files/uClinux-2.4.x.tar.gz","uClinux-2.4.x.tar.gz",
		"You need to use these sources with current patch <br>		
		=> <a href='http://sourceforge.net/project/showfiles.php?group_id=108103&package_id=122923'>
		latest package as a tar file</a><br>
		=> <a href='http://cvs.sourceforge.net/viewcvs.py/linav/kernel/uClinux-2.4.x/'>Browse cvs</a>,
		cvs module: kernel, path in module: kernel/uClinux-2.4.x");
/*	addFile("files/microwindows-0.90.tar.gz","microwindows-0.90.tar.gz",
		"Source of microwindows/nano-x<br>		
		=> <a href='http://sourceforge.net/project/showfiles.php?group_id=108103&package_id=124740'>
		latest package as a tar file</a><br>
		=> <a href='http://cvs.sourceforge.net/viewcvs.py/linav/gui/microwindows-0.90/'>Browse cvs</a>,
		cvs module: kernel, path in module: microwindows-0.90/");
*/
/*
	addFile("files/jpegsrc.v6b.tar.gz","jpegsrc.v6b.tar.gz",
		"libjpeg for nano-x, to build, untar file in gui dir, copy makefile.ansi to makefile,
		run configure and then make.");
	addFile("files/libpthread.a","libpthread.a",
		"corrected version of libpthread, after installing the toolchain from uClinux, 
		copy this file in /usr/local/arm-elf/lib/");
*/
	addFile("files/linux.bin","linux.bin",
		"Latest build of the kernel for the av");
	addFile("files/rom.inc","rom.inc",
                "this is an initial romdisk used to boot the kernel, put this file in arch/armnommu/mach-av3xx");
////////////////////////////////////////////////////////////////
	$doc->endDisp();
?>
	</td>
	</tr>
	</table>
<?
	//$page->fin();
	//finPage();
?>
