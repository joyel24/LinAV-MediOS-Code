void Crc_CorrectRomSettings()
{
  switch (Vnes.var.crc)
  {
/*  
 	case 0x0b5667e9:		// 000 Excitebike (E)
	case 0x3a94fa0b:		// 000 Excitebike (JU)
	case 0x16342ad0:		// 000 baltron(J)
		Vnes.var.deccycle=0;
		Vnes.var.cpucycle = 112;		
	break;
	case 0x900c7442:		// 001 Konamic Sports in Seoul (J)
		//BANKSWITCH_PER_TILE = 1;
	break;	
	case 0xdb564628:		// 001 Mario Open Golf (J)
		Vnes.var.deccycle=0;
		Vnes.var.cpucycle = 105;
	break;
	case 0x547ad451:	// Gekitou Stadium!! (J)
		Vnes.var.deccycle=0;
		Vnes.var.cpucycle = 111;
	break;
	case 0xaf16ee39:	// Hyokkori Hyoutan Jima - Nazo no Kaizokusen (J)
		Vnes.var.deccycle=0;
		Vnes.var.cpucycle = 112;
	break;
	case 0x9183054e:	// Dengeki - Big Bang! (J)
		Vnes.var.deccycle=0;
		Vnes.var.cpucycle = 115;
	break;
	case 0x7e5d2f1a:	// Itadaki Street - Watashi no Mise ni Yottette (J)
	case 0x37bf04d7:	// Marusa no Onna (J)
		Vnes.var.deccycle=0;
		Vnes.var.cpucycle = 120;
	break;
	case 0x11469ce3:	// Viva! Las Vegas (J)
		Vnes.var.deccycle=0;
		Vnes.var.cpucycle = 130;
	break;
	case 0x58507bc9:	// Famicom Top Management (J)
		Vnes.var.deccycle=0;
		Vnes.var.cpucycle = 160;
	break;
	case 0x7bd7b849:	// Nekketu koukou Dodgeballbu (J)
		Vnes.var.deccycle=0;
		Vnes.var.cpucycle = 170;
	break;
		// Mapper 002 -----------------------------------------------------------------
	case 0x7b0a41b9:	// Esper Bouken Tai (J)
		Vnes.var.deccycle=0;
		Vnes.var.cpucycle = 128;
	break;
	case 0xb59a7a29:	// Guardic Gaiden (J)
		Vnes.var.deccycle=0;
		Vnes.var.cpucycle = 136;
	break;
		// Mapper 003 -----------------------------------------------------------------
	case 0xca26a0f1:	// Family Trainer - Dai Undoukai (J)
		Vnes.var.deccycle=0;
		Vnes.var.cpucycle = 113;
	break;
	case 0xa30d8baf:	// Tokoro San no Mamorumo Semerumo (J)
		Vnes.var.deccycle=0;
		Vnes.var.cpucycle = 115;
		CYCLES_BEFORE_NMI = 10;
		//BANKSWITCH_PER_TILE = 1;
	break;
	case 0xd21da4f7:	// Drop Zone (E)
		Vnes.var.deccycle=0;
		Vnes.var.cpucycle = 105;
	break;
		// Mapper 004 -----------------------------------------------------------------		
	case 0x630be870:   //Crystalis  (U)
		Vnes.var.cpucycle=112;
  		Vnes.var.deccycle=00;
  	break;
	case 0xa0ed7d20:   //Super Mario Bros. 3 (PRG 1 CHR 0)  (U)
		Vnes.var.cpucycle=113;
  		Vnes.var.deccycle=00;
  	break;
	case 0x1d8b2f59:	// Hyryuu no Ken 3 (J)
		Vnes.var.deccycle=0;
		Vnes.var.cpucycle = 140;
	break;
	case 0x14a01c70:	// Gun-Dec (J)
		//BANKSWITCH_PER_TILE = 1;
	break;
	case 0xb42feeb4:	// Beauty and the Beast (E)
		//			Vnes.var.deccycle=0;
		Vnes.var.cpucycle = 108;
	break;
	case 0x0baf01d0:	// Juju Densetsu (J)
		Vnes.var.deccycle=0;
		Vnes.var.cpucycle = 112;
	break;
	case 0x5c707ac4:	// Mother (J)
		Vnes.var.deccycle=0;
		Vnes.var.cpucycle = 114;
		//BANKSWITCH_PER_TILE = 1;
	break;
	case 0xcb106f49:	// F-1 Sensation (J)
		Vnes.var.deccycle=0;
		Vnes.var.cpucycle = 115;
	break;
	case 0x8b59bac3:	// Radia Senki - Reimei Hen (J)
	case 0x00161afd:	// Ys 2 - Ancient Ys Vanished The Final Chapter (J)
	case 0x26ff3ea2:	// Yume Penguin Monogatari (J)
		Vnes.var.deccycle=0;
		Vnes.var.cpucycle = 120;
	break;
	case 0x459adfa8:	// Cattou Ninden Teyandee (J)
	case 0xbea682e5:	// Doki! Doki! Yuuenchi (J)
	case 0xf1081b1b:	// Heavy Barrel (J)
	case 0xb4d554d6:	// Heavy Barrel (U)
	case 0x9a172152:	// Jackie Chan (J)
	case 0x170739cf:	// Jigoku Gokuraku Maru (J)
	case 0xdeddd5e5:	// Kunio Kun no Nekketsu Soccer League (J)
	case 0x0c935dfe:	// Might and Magic (J)
	case 0x85c5b6b4:	// Nekketsu Kakutou Densetsu (J)
	case 0x6bbd9f11:	// Utsurun Desu (J)
	case 0x23f4b48f:	// Wily & Light no Rockboard - That's Paradise (J)
		Vnes.var.deccycle=0;
		Vnes.var.cpucycle = 128;
	break;
	case 0xd88d48d7:	// Kick master(U)
		Vnes.var.deccycle=0;
		Vnes.var.cpucycle = 130;
	break;
	case 0xa9a0d729:	// Dai Kaijuu - Deburas (J)
	case 0x346cd5d3:	// Ike Ike! Nekketsu Hockey Bu - Subette Koronde Dai Rantou (J)
	case 0x5fbd9178:	// Mitsume ga Tooru (English v1_01)
	case 0x390e0320:	// Mitsume ga Tooru (J)
	case 0x6055fe9b:	// Tecmo Super Bowl (J)
	case 0x8e5c2818:	// Tecmo Super Bowl (U)
	case 0xe3765667:	// Top Gun - The Second Mission (U)
		Vnes.var.deccycle=0;
		Vnes.var.cpucycle = 136;
	break;
	case 0x0ae5b57f:	// Super Donkey Kong 2 (Unk)
		Vnes.var.deccycle=0;
		Vnes.var.cpucycle = 140;
	break;
	case 0x336e2a6f:	// Asmik Kun Land (J)
	case 0x7ca52798:	// Chiki Chiki Machine Mou Race (J)
	case 0xc17ae2dc:	// God Slayer - Haruka Tenkuu no Sonata (J)
	case 0x8685f366:	// Matendouji (J)
	case 0xb55da544:	// Nekketsu Koukou - Dodgeball Bu - Soccer Hen (J)
	case 0x4fb460cd:	// Nekketsu! Street Basket - Ganbare Dunk Heroes (J)
	case 0xe19a2473:	// Sugoro Quest - Dice no Senshi Tachi (J)
	case 0x14a45522:	// Sugoro Quest - The Quest of Dice Heros (English vX.XX)
	case 0x80cd1919:	// Super Mario Bros 3 (E)
	case 0xe386da54:	// Super Mario Bros 3 (J)
	case 0xd852c2f7:	// Time Zone (J)
	case 0xade11141:	// Wanpaku Kokkun no Gourmet World (J)
		Vnes.var.deccycle=0;
		Vnes.var.cpucycle = 144;
	break;
	case 0x3c5ae54b:	// 1999 - Hore, Mitakotoka! Seikimatsu (J)
	case 0xb548fd2e:	// Bananan Ouji no Dai Bouken (J)
	case 0xeffeea40:	// Klax (J)
	case 0x2dc3817d:	// Meimon! Takonishi Ouendan - Kouha 6 Nin Shuu (J)
	case 0x5c1d053a:	// SD Battle Oozumou - Heisei Hero Basho (J)
	case 0x0bf31a3d:	// Soreike! Anpanman - Minna de Hiking Game! (J)
		Vnes.var.deccycle=0;
		Vnes.var.cpucycle = 152;
	break;
	case 0x6cc62c06:	// HoshinoKirby-YumenoIzuminoMonogatari(J)
	case 0xa67ea466:	// Arean 3
		Vnes.var.deccycle=0;
		Vnes.var.cpucycle = 110;
	break;
		// Mapper 007 -----------------------------------------------------------------
	case  0x524a5a32:	// Battletoads (E)
	case 0x9806cb84:	// Battletoads (J)
	case 0x279710dc:	// Battletoads (U)
		Vnes.var.deccycle=0;
		Vnes.var.cpucycle = 112;
	break;
	case 0x23d7d48f:	// Battletoads Double Dragon(E)
	case 0xceb65b06:	// Battletoads Double Dragon(U)
		Vnes.var.deccycle=0;
		Vnes.var.cpucycle = 111;
	break;
		// Mapper 016 -----------------------------------------------------------------
	case 0x6c6c2feb:	// Dragon Ball 3 - Gokuu Den (J)
	case 0x0c187747:	// Nishimura Kyoutarou Mystery - Blue Train Satsujin Jiken (J)
	case 0x09499f4d:	// DragonBallZ3-RessenJinzouNingen(J)
		Vnes.var.deccycle=0;
		Vnes.var.cpucycle = 120;
	break;
	case 0xab3062cf:	// sakigake
		Vnes.var.deccycle=0;
		Vnes.var.cpucycle = 110;
	break;
		//	case 0x73ac76db )  // SD Gundam Gaiden 2
		//  Vnes.var.deccycle=0;
		Vnes.var.cpucycle = 120;
		// Mapper 018 -----------------------------------------------------------------
	case 0xa54d9086:	// Toukon Club (J)
		Vnes.var.deccycle=0;
		Vnes.var.cpucycle = 114;
	break;
		// Mapper 019 -----------------------------------------------------------------
	case 0x17421900:	// Youkai Douchuuki (J)
		//BANKSWITCH_PER_TILE = 1;
	break;
	case 0x6901346e:	// Sangokushi 2 - Haou no Tairiku (J)
		Vnes.var.deccycle=0;
		Vnes.var.cpucycle = 105;
	break;
	case 0x761ccfb5:	// Digital Devil Story - Megami Tensei 2 (J)
		Vnes.var.deccycle=0;
		Vnes.var.cpucycle = 119;
	break;
	case 0x9a2b0641:	// Namco Classic 2 (J)
		Vnes.var.deccycle=0;
		Vnes.var.cpucycle = 120;
	break;
	case 0x2b825ce1:	// Namco Classic (J)
		//	   	case 0x3deac303:	// Rolling Thunder (J)
	case 0xb62a7b71:	// Family Circuit '91
		Vnes.var.deccycle=0;
		Vnes.var.cpucycle = 144;
	break;
	case 0x96533999:	// Dokuganryuu Masamune (J)
		CYCLES_BEFORE_NMI = 10;
	break;
	case 0x3deac303:	// RollingThunder(J)
		Vnes.var.deccycle=0;
		Vnes.var.cpucycle = 150;
	break;
		// Mapper 021 -----------------------------------------------------------------
	case 0xb201b522:	// Wai Wai World 2 - SOS!! Paseri Jou (J)
		Vnes.var.deccycle=0;
		Vnes.var.cpucycle = 117;
	break;
		// Mapper 023 -----------------------------------------------------------------
	case 0x64818fc5:	// Wai Wai World (J)
		Vnes.var.deccycle=0;
		Vnes.var.cpucycle = 115;
	break;
		// Mapper 024 -----------------------------------------------------------------
	case 0xba6bdd6a:	// Akumajou Densetsu (J)
		Vnes.var.deccycle=0;
		Vnes.var.cpucycle = 119;
	break;		
		// Mapper 032 -----------------------------------------------------------------
	case 0x788bed9a:	// Meikyuu Jima (J)
		Vnes.var.deccycle=0;
		Vnes.var.cpucycle = 114;
	break;
		// Mapper 033 -----------------------------------------------------------------
	case 0xa71c3452:	// Insector X (J)
		Vnes.var.deccycle=0;
		Vnes.var.cpucycle = 110;
	break;
	case 0x63bb86b5:	// Jetsons-Cogswell'sCaper!,The(J)
		Vnes.var.deccycle=0;
		Vnes.var.cpucycle = 117;
	break;		
		// Mapper 065 -----------------------------------------------------------------
	case 0xd202612b:	// Spartan X 2 (J)
		Vnes.var.deccycle=0;
		Vnes.var.cpucycle = 125;
	break;		
		// Mapper 094 -----------------------------------------------------------------
	case 0x441aeae6:	// Senjou no Ookami (J)
		Vnes.var.deccycle=0;
		Vnes.var.cpucycle = 117;
	break;
		// Mapper 095 -----------------------------------------------------------------
	case 0xc7a79be2:	// Dragon Buster (J)
		Vnes.var.deccycle=0;
		Vnes.var.cpucycle = 110;
	break;
		// Mapper 246 -----------------------------------------------------------------
	case 0xea76fb00:	//
		Vnes.var.deccycle=0;
		Vnes.var.cpucycle = 112;
	break;
		
		// Special
	case 0xbc7485b5:    // Elite - Ian Bell
		Vnes.var.fps=50;
	break;
   		*/
   
// data base to set the CYCLES PER LINE

// Mapper 000 -----------------------------------------------------------------

   case 0x0b5667e9:  // Excitebike (E)
   case 0x3a94fa0b:   // Excitebike (JU)
	Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 112;
	break;
   case 0x57970078: 
   case 0x89fdac93: // f1 race(J)
  	Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 115;
  	break;
   case 0x5cf548d3:
   case 0x0b97b2af: // super mario bros. (J)
  	Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 130;
	break;
   case 0x90ca616d:   //Duck Hunt
   case 0x00000000:
   case 0xeeb7a62b:
   case 0xad6cdf29:
   case 0x41cf3616:
   case 0xed588f00:
   case 0x42d893e4:  //operation wolf
   case 0x1388aeb9:
   	Vnes.var.padmode=2;
   	break;
// Mapper 001 -----------------------------------------------------------------
   case 0x900c7442:   // Konamic Sports in Seoul (J)
  	//BANKSWITCH_PER_TILE = 1;
  	break;
   case 0xcf9cf7a2:   // Romancia (J)
  	//DPCM_IRQ = 1;
  	break;
   case 0xdb564628:   // Mario Open Golf (J)
  	Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 105;
  	break;
   case 0x547ad451:   // Gekitou Stadium!! (J)
	Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 111;
	break;
   case 0xaf16ee39:   // Hyokkori Hyoutan Jima - Nazo no Kaizokusen (J)	
  	Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 112;
	break;
   case 0x9183054e:   // Dengeki - Big Bang! (J)
	Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 115;
	break;
   case 0x7e5d2f1a:  // Itadaki Street - Watashi no Mise ni Yottette (J)
   case 0x37bf04d7:   // Marusa no Onna (J)
  	Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 120;
	break;
   case 0x11469ce3:   // Viva! Las Vegas (J)
  	Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 130;
	break;
   case 0x58507bc9:   // Famicom Top Management (J)
  	Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 160;
	break;
// Mapper 002 -----------------------------------------------------------------
   case 0x7b0a41b9:   // Esper Bouken Tai (J)
  	Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 128;
	break;
   case 0xb59a7a29:   // Guardic Gaiden (J)
  	Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 136;
	break;
   case 0x2f66e302:   //California Games (U)
   	Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 114;
	break;
// Mapper 003 -----------------------------------------------------------------
   case 0xca26a0f1:  // Family Trainer - Dai Undoukai (J)
  	Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 113;
	break;
   case 0xa30d8baf:   // Tokoro San no Mamorumo Semerumo (J)
  	Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 115;
 	CYCLES_BEFORE_NMI = 10;
  	//BANKSWITCH_PER_TILE = 1;
	break;
// Mapper 004 -----------------------------------------------------------------   
   case 0x14a01c70:   // Gun-Dec (J)
  	//BANKSWITCH_PER_TILE = 1;
	break;
   case 0xb42feeb4:   // Beauty and the Beast (E)
  	Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 107;
	break;
   case 0x08fe3b15:  //Garou Densetsu Special (J)
        Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 112;
	break;
   case 0x0baf01d0:   // Juju Densetsu (J)
  	Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 112;
	break;
   case 0x6cc62c06:  // hoshi no kirby (J)
   case 0x9077a623:  // kirby's adventure (U)   
   case 0x014a755a:  // kirby's adventure (E)
   	Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 120;
	break;
   case 0x80cd1919:  // Super Mario Bros 3 (E)
   case 0xe386da54:  // Super Mario Bros 3 (J)
   case 0xa0ed7d20:  // Super Mario Bros 3 (U)
   case 0xc78d17d3:  // same ...
  	Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 120;
  	//BANKSWITCH_PER_TILE = 1;
	break;
   case 0xf0c77dcb: //Ninja Gaiden 3< - The Ancient Ship of Doom> (U)  
        Vnes.var.deccycle=0;
        Vnes.var.cpucycle=120;	
        break;
   /*case 0xc43da8e2: //Ninja Gaiden 2< - The Dark Sword of Chaos> (U)
        Vnes.var.deccycle=0;
        Vnes.var.cpucycle=114;	
        //CYCLES_BEFORE_NMI = 10;
        break;        */
   case 0x5c707ac4:   // Mother (J)   
  	Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 114;
  	//BANKSWITCH_PER_TILE = 1;
	break;
   case 0xcb106f49:  // F-1 Sensation (J)
   case 0x19004d03:   // Evil Kirby (Hack)
  	Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 115;
	break;
   case 0x8b59bac3:  // Radia Senki - Reimei Hen (J)
   case 0x00161afd:  // Ys 2 - Ancient Ys Vanished The Final Chapter (J)
   case 0x26ff3ea2:   // Yume Penguin Monogatari (J)
  	Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 120;
	break;
   case 0x459adfa8:  // Cattou Ninden Teyandee (J)
   case 0xbea682e5:  // Doki! Doki! Yuuenchi (J)
   case 0xf1081b1b:  // Heavy Barrel (J)
   case 0xb4d554d6:  // Heavy Barrel (U)
   case 0x9a172152:  // Jackie Chan (J)
   case 0x170739cf:  // Jigoku Gokuraku Maru (J)
   case 0xdeddd5e5:  // Kunio Kun no Nekketsu Soccer League (J)
   case 0x0c935dfe:  // Might and Magic (J)
   case 0x85c5b6b4:  // Nekketsu Kakutou Densetsu (J)
   case 0x6bbd9f11:  // Utsurun Desu (J)
   case 0x23f4b48f:   // Wily & Light no Rockboard - That's Paradise (J)
  	Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 128;
	break;
   case 0xa9a0d729:  // Dai Kaijuu - Deburas (J)
   case 0x346cd5d3:  // Ike Ike! Nekketsu Hockey Bu - Subette Koronde Dai Rantou (J)
   case 0x5fbd9178:  // Mitsume ga Tooru (English v1_01)
   case 0x390e0320:  // Mitsume ga Tooru (J)
   case 0x6055fe9b:  // Tecmo Super Bowl (J)
   case 0x8e5c2818:  // Tecmo Super Bowl (U)
   case 0xe3765667:   // Top Gun - The Second Mission (U)
  	Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 136;
	break;
   case 0x28cb6c6b:   // Hi-Game 1999 Super Donkey Kong 2 (Pirate).nes
  	Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 140;
	break;
   case 0x0ae5b57f:  // Super Donkey Kong 2 (Unk)
   case 0x63e09bba:   // Super Donkey Kong 2 (Unk) (Alt)
  	Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 140;
	break;
   case 0x336e2a6f:  // Asmik Kun Land (J)
   case 0x7ca52798:  // Chiki Chiki Machine Mou Race (J)
   case 0xc17ae2dc:  // God Slayer - Haruka Tenkuu no Sonata (J)
   //case 0x6cc62c06:  // hoshi no kirby (J)
   //case 0x9077a623:  // kirby's adventure (E)
   case 0x8685f366:  // Matendouji (J)
   case 0xb55da544:  // Nekketsu Koukou - Dodgeball Bu - Soccer Hen (J)
   case 0x4fb460cd:  // Nekketsu! Street Basket - Ganbare Dunk Heroes (J)
   case 0xe19a2473:  // Sugoro Quest - Dice no Senshi Tachi (J)
   case 0x14a45522:  // Sugoro Quest - The Quest of Dice Heros (English vX.XX)
   //case 0x80cd1919:  // Super Mario Bros 3 (E)
   //case 0xe386da54:  // Super Mario Bros 3 (J)
   //case 0xa0ed7d20:  // Super Mario Bros 3 (E)
   case 0xd852c2f7:  // Time Zone (J)
   case 0xade11141:   // Wanpaku Kokkun no Gourmet World (J)
  	Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 144;
	break;
   case 0x630be870: // Crystails 
  	Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 148;
	break;
   case 0x3c5ae54b:  // 1999 - Hore, Mitakotoka! Seikimatsu (J)
   case 0xb548fd2e:  // Bananan Ouji no Dai Bouken (J)
   case 0xeffeea40:  // Klax (J)
   case 0x2dc3817d:  // Meimon! Takonishi Ouendan - Kouha 6 Nin Shuu (J)
   case 0x5c1d053a:  // SD Battle Oozumou - Heisei Hero Basho (J)
   case 0x0bf31a3d:   // Soreike! Anpanman - Minna de Hiking Game! (J)
  	Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 152;
	break;
   case 0xaafe699c: // Ninja Ryukenden 3 
  	Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 120;
	break;
   case 0x80fb117e: // startropics 2
  	Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 112;
	break;
   /*case 0x59a706e4: //gun nac (U)
   case 0xa0b8d310: //gun nac (J)
   	CYCLES_BEFORE_NMI = 7;
   	break;*/
// Mapper 007 -----------------------------------------------------------------
   case 0x524a5a32:  // Battletoads (E)
   case 0x9806cb84:  // Battletoads (J)
   case 0x279710dc:   // Battletoads (U)
  	Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 110;
	//CYCLES_BEFORE_NMI = 1;
	break;

// Mapper 016 -----------------------------------------------------------------
   case 0x6c6c2feb:  // Dragon Ball 3 - Gokuu Den (J)
   case 0x0c187747:   // Nishimura Kyoutarou Mystery - Blue Train Satsujin Jiken (J).
  	Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 122;
	break;
// Mapper 018 -----------------------------------------------------------------
   case 0xa54d9086:   // Toukon Club (J)
  	Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 114;
	break;
// Mapper 019 -----------------------------------------------------------------
   case 0x17421900:   // Youkai Douchuuki (J)
  	//BANKSWITCH_PER_TILE = 1;
	break;
   case 0x6901346e:   // Sangokushi 2 - Haou no Tairiku (J)
  	Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 105;
	break;
   case 0x761ccfb5:   // Digital Devil Story - Megami Tensei 2 (J)
  	Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 119;
	break;
   case 0x9a2b0641:   // Namco Classic 2 (J)
  	Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 120;
	break;
   case 0x2b825ce1:  // Namco Classic (J)
   case 0x3deac303:   // Rolling Thunder (J)
  	Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 144;
	break;
   case 0x96533999:   // Dokuganryuu Masamune (J)
  	CYCLES_BEFORE_NMI = 10;
	break;
// Mapper 020 -----------------------------------------------------------------
   case 0xb24b4e4d:  // Kinnikuman - Kinnikusei Oui Soudatsusen
   case 0x014c4e4b:  // Legend of Zelda 2 - Link no Bouken, The
   case 0xc1414c4e:   // Super Boy Allan
  	Vnes.var.deccycle=0;
	Vnes.var.cpucycle =117;
	break;
   case 0xb646424d: // firebam
  	Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 160;
	break;
// Mapper 021 -----------------------------------------------------------------
   case 0xb201b522:   // Wai Wai World 2 - SOS!! Paseri Jou (J)
  	Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 117;
	break;
// Mapper 023 -----------------------------------------------------------------
   case 0x64818fc5:   // Wai Wai World (J)
  	Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 115;
	break;
// Mapper 024 -----------------------------------------------------------------
   case 0xba6bdd6a:   // Akumajou Densetsu (J)
	Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 119;	
	break;
// Mapper 032 -----------------------------------------------------------------
   case 0x788bed9a:   // Meikyuu Jima (J)
  	Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 114;
	break;
// Mapper 065 -----------------------------------------------------------------
   case 0xd202612b:   // Spartan X 2 (J)
  	Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 125;
	break;
// Mapper 079 -----------------------------------------------------------------	
   case 0x460a9cb0:  //Mermaid of Atlantis (U)
   	break;
// Mapper 088 -----------------------------------------------------------------
   case 0xc1b6b2a6:   // Devil Man (J)
	break;
case 0xd9803a35:
  Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 180;
	break;
// Mapper 094 -----------------------------------------------------------------
   case 0x441aeae6:   // Senjou no Ookami (J)
  	Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 117;
	break;
// Mapper 118 -----------------------------------------------------------------
   case 0x3b0fb600: // Ys 3 Wanderers From Ys (J)
  	Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 140;
	break;
// Mapper 246 -----------------------------------------------------------------
   case 0xea76fb00:   // Fong Shen Bang
  	Vnes.var.deccycle=0;
	Vnes.var.cpucycle = 113;
	break;
//special
   case 0xbc7485b5:    // Elite - Ian Bell
	Vnes.var.fps=50;
	break;
	
  }
}
