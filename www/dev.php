<?

	require("include/enteteHtml.php");

	require("include/affBlock.class");
	require("include/dispItem.class");
	require("include/affPage.class");
	require("include/configTable.inc");

	entete();

	//$titre2="Developers";
	//$page=new affPage($titre,$titre2);

	function addDev($pseudo,$name,$mail,$dbo,$adress,$Employement,$homePage,$skills)
	{
		global $doc,$CoTR,$CoHead,$CoCel;
		$str="<table class='forumline' width='300'><tr bgcolor=$CoTR><td bgcolor=$CoHead>";
		$str.="<center>";
		$str .="$pseudo<br>\n";
		$str.="</center></td></tr>";

		$str.="<tr bgcolor=$CoTR><td bgcolor=$CoCel>";
		if($name!="")
			$str.="<u>Real name</u>: $name<br>\n";
		if($mail!="")
			$str.="<u>Email</u>: $mail<br>\n";
		if($dbo != "")
			$str.="<u>Date of birth</u>: $dbo<br>\n";
		if($adress!="")
			$str.="<u>Living</u>: $adress<br>\n";
		if($Employement!="")
			$str.="<u>Employment</u>: $Employement<br>\n";
		if($homePage!="")
			$str.="<u>Home page</u>: <a href='$homePage' target='_blank'>$homePage</a><br>\n";
		if($skills!="")
			$str.="<u>Tech Skills</u>: $skills<br>\n";

		$str.="</td></tr></table>";

		$doc->addItem($str);
	}
?>
	<table>
	<tr>
	<td>
<?

	$doc=new dispItem("Developers");
/////////////////////////////////////////////////////////////////
	addDev("oxyGen77",
		   "Christophe THOMAS",
		   "oxygen77@free.fr",
		   "13th august 1977",
		   "Paris, FRANCE",
		   "none",
		   "",
		   "Java, C/C++, PHP, LaTex, ARM assembly"
		   );

	addDev("Sorg",
		   "Timothée GROS",
		   "sorg@netcourrier.com",
		   "07-02-81",
		   "Paris",
		   "Engineer",
		   "",
		   "C, PHP, x86, 68000 assembly"
		   );
	addDev("Schoki",
                   "Goetz Minuth",
                   "minuth@gmx.de",
                   "18.05.1970",
                   "Near Nuremberg",
                   "Developer",
                   "",
		   "c/c++, VBA, DCOM"
                   );
	addDev("Midk",
                   "Zakk Roberts",
                   "midkay@gmail.com",
                   "15.05.1990",
                   "Seattle, wa",
                   "",
                   "",
                   "c"
                   );
	addDev("McLeodz",
                   "Mounier jeremy",
                   "jeremymounier@yahoo.fr",
                   "07.12.1984",
                   "St etienne (FRANCE)",
                   "student / work in IT",
                   "http://www.raidskrew.com",
                   "C, network, webdesign"
                   );


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
