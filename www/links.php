<?
	require("include/enteteHtml.php");
	require("include/affBlock.class");
	require("include/dispItem.class");

	require("include/affPage.class");
	require("include/configTable.inc");

	entete();
	//$titre2="Links";
	//$page=new affPage($titre,$titre2);

	function addLinks($url,$name,$txt)
	{
		global $links;
		$links->addItem("<a href='$url' target='_blank'>$name</a>",$txt);
	}

?>
	<table width='100%'>
	<tr>
	<td>
<?

	$links=new dispItem("Links");
/////////////////////////////////////////////////////////////////
	addLinks("http://www.uclinux.org","http://www.uclinux.org",
		"Home page of uclinux");
	addLinks("http://www.ucdot.org","http://www.ucdot.org",
		"uCdot - Embedded Linux Developer Forum");
	addLinks("http://uclinux.home.at/","http://uclinux.home.at/",
		"The uClinux Directory");
	addLinks("http://www.arm.linux.org.uk","http://www.arm.linux.org.uk",
		"The ARM Linux Project");
	addLinks("","","<b>Other related projects</b>");
	addLinks("http://avos.sf.net/","http://avos.sf.net/",
		"Home page of avos' project");
	addLinks("http://www.sf.net/projects/garme","http://www.sf.net/projects/garme",
		"Home page of garme's project");
	addLinks("http://www.donat.org/archos/","http://www.donat.org/archos/",
                "GmEmu (new os for gmini)");
	addLinks("","","<b>Our Yahoo Group</b>");
	addLinks("http://groups.yahoo.com/group/linav/","http://groups.yahoo.com/group/linav/",
		"Our yahoo's group");
	addLinks("","","<b>If you need parts for your Archos device</b>");
	addLinks("http://www.newmp3technology.net/","http://www.newmp3technology.net/",
                "newMp3Technology");
////////////////////////////////////////////////////////////////
	$links->endDisp();
?>
	</td>
	</tr>
	</table>
<?
	//$page->fin();
	//finPage();
?>

