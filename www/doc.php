<?
	require("include/enteteHtml.php");
	require("include/affBlock.class");
	require("include/dispItem.class");

	require("include/affPage.class");
	require("include/configTable.inc");

	entete();
	//$titre2="DOC";
	//$page=new affPage($titre,$titre2);


	function addDoc($url,$name,$txt)
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
	addDoc("doc/Porting_Linux_to_ARM.pdf","Porting_Linux_to_ARM.pdf",
		"Porting the Linux kernel to a new ARM Platform, from Aleph One");
	addDoc("http://adam.kaist.ac.kr/~hschoe/Getting_Familiar_with_uClinuxARM2_6.html","Linux 2.6 on ARM",
		"Getting Familiar with uClinux/ARM 2.6");

	addDoc("","","<font color='red'><b>Hardware</b></font>");
	addDoc("doc/ARM_Architecture_Reference_Manual.pdf","ARM ref. manual",
		"ARM Architecture Reference Manual");
	addDoc("doc/TMS320DSC24.pdf","TMS320DSC24-1","TMS320DSC24 data manual from TI");
	addDoc("doc/TMS320DSC24-spru574.pdf","TMS320DSC24-2","TMS320DSC24 complete manual from TI");

	addDoc("","","<font color='red'><b>Doc from avos</b></font>");
	addDoc("http://avos.sourceforge.net/avmap.txt",
			"AV300 hardware port map v0.01","Hardware low level port access map By Doggermoore");
	addDoc("http://avos.sourceforge.net/av300_osd.txt","av300_osd.txt",
			"A description of the av300 on screen display by DoGgEr");
	adddoc("http://avos.sourceforge.net/av300_mem.txt","av300_mem.txt",
			"AV300 memory map");

	addDoc("","","<font color='red'><b>Bill Gatliff's doc (<a	href='http://billgatliff.com/articles.shtml' target='_blank'>website</a>)</b></font>");
	addDoc("doc/kernel_interrupts_bill_gatliff.pdf","interups.pdf",
			"The Linux 2.4 Kernel's Interrupt Controller API");
	addDoc("doc/kernel_mmu_bill_gatliff.pdf","MMU",
			"The Linux 2.4 Kernel's Memory Management Unit API");
	addDoc("doc/kernel_startup_bill_gatliff.pdf","Startup",
			"The Linux 2.4 Kernel's Startup Procedure");







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
