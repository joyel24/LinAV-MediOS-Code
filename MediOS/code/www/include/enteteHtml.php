<?

	//*************************
	// Entete html
	//
	// THOMAS Christophe
	//*************************

	if(!isset($entete_html)) // éviter les inclusions successives
	{
		$entete_html=1;


	//************************
	// <head> </head> et <body>
	//************************

	// TODO: titre par défaut + author + keywords

	function entete($titre='',$paramBody='',$cssAddon='')
	{
?>
	<html>
		<head>
			<META http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
   			<META NAME="Author" CONTENT="">
   			<META NAME="Keywords" content="">
<?
		if($titre=="")
			$titre="LinAV - uClinux for archos multimedia device";

		echo "<TITLE>$titre</TITLE>\n";

		//if($cssAddon=='')
			echo "<LINK REL='Stylesheet' HREF='/style.css'  TYPE='text/css'>\n";
		//else
			echo "\n" . $cssAddon . "\n";

		if($paramBody == "with_html_editor")
		{
			require("editor.inc");
			$paramBody=" onload='HTMLArea.replaceAll()' ";
		}

		echo "<BODY text='#000000' bgcolor='#FFFFFF' background='/images/back.gif' $paramBody >\n";

	}

	function finPage()
	{
?>
		</BODY>
	</HTML>
<?
		exit;
	}

	} // fin test inclusion
?>
