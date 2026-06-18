<?
	require("include/enteteHtml.php");
	require("include/affBlock.class");
	require("include/dispItem.class");

	require("include/affPage.class");
	require("include/configTable.inc");

	entete();
//	$titre2="HOWTOS";
//	$page=new affPage($titre,$titre2);


	function addHowTo($url,$name,$txt)
	{
		global $howto;
		if($url!="")
			$howto->addItem("<a href='$url' target='_blank'>$name</a>",$txt);
		else
			$howto->addItem("",$txt);
	}

?>
	<table width='100%'>
	<tr>
	<td>
<?

	$howto=new dispItem("HowTos");
/////////////////////////////////////////////////////////////////
	/*examples
	addHowTo("howto/file.pdf","file.pdf",
		"description");
	*/
        addHowTo("howto/toolchain/toolchain.html","toolchain.html",
                "HOW-TO build the arm-linux Toolchain.");
	addHowTo("howto/garme/garme.html","garme.html",
                "HOW-TO build GARME (AV3xx Emulator).");


////////////////////////////////////////////////////////////////
	$howto->endDisp();
?>
	</td>
	</tr>
	</table>
<?
//	$page->fin();
//	finPage();
?>
