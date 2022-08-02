modded class CharacterCreationMenu extends UIScriptedMenu
{

	override Widget Init()
	{

		layoutRoot = GetGame().GetWorkspace().CreateWidgets( "Colorful-UI/gui/layouts/new_ui/character_creation/pc/Colorful.character_creation.layout" );

		m_CharacterRotationFrame			= layoutRoot.FindAnyWidget( "character_rotation_frame" );
		m_Apply								= layoutRoot.FindAnyWidget( "apply" );
		m_Save 								= layoutRoot.FindAnyWidget( "save" );
		m_RandomizeCharacter				= layoutRoot.FindAnyWidget( "randomize_character" );
		m_BackButton						= layoutRoot.FindAnyWidget( "back" );
		m_Version							= TextWidget.Cast( layoutRoot.FindAnyWidget( "version" ) );
		m_DetailsRoot 						= layoutRoot.FindAnyWidget( "menu_details_tooltip" );
		m_DetailsLabel						= TextWidget.Cast( m_DetailsRoot.FindAnyWidget( "menu_details_label" ) );
		m_DetailsText						= RichTextWidget.Cast( m_DetailsRoot.FindAnyWidget( "menu_details_tooltip_content" ) );
		m_CharacterHeaderText 				= TextWidget.Cast(layoutRoot.FindAnyWidget( "char_header_text" ));
		m_PlayedCharacterInfo 				= layoutRoot.FindAnyWidget( "played_char_info" );
		
		string version;
		GetGame().GetVersion( version );
		#ifdef PLATFORM_CONSOLE
			version = "#main_menu_version" + " " + version + " (" + g_Game.GetDatabaseID() + ")";
		#else
			version = "#main_menu_version" + " " + version;
		#endif
		m_Version.SetText( version );
		
		if( m_Scene && m_Scene.GetIntroCharacter() )
		{
			m_OriginalCharacterID = m_Scene.GetIntroCharacter().GetCharacterID();
		}
		
		m_NameSelector		= new OptionSelectorEditbox( layoutRoot.FindAnyWidget( "character_name_setting_option" ), m_Scene.GetIntroCharacter().GetCharacterName(), null, false );
		m_GenderSelector	= new OptionSelectorMultistateCharacterMenu( layoutRoot.FindAnyWidget( "character_gender_setting_option" ), 0, null, false, m_Scene.GetIntroCharacter().GetCharGenderList() );
		if ( m_Scene.GetIntroCharacter().IsCharacterFemale() )
		{
			m_GenderSelector.SetValue( "Female" );
			m_SkinSelector	= new OptionSelectorMultistateCharacterMenu( layoutRoot.FindAnyWidget( "character_head_setting_option" ), 0, null, false, m_Scene.GetIntroCharacter().GetCharList( ECharGender.Female ) );
		}
		else
		{
			m_GenderSelector.SetValue( "Male" );
			m_SkinSelector	= new OptionSelectorMultistateCharacterMenu( layoutRoot.FindAnyWidget( "character_head_setting_option" ), 0, null, false, m_Scene.GetIntroCharacter().GetCharList( ECharGender.Male ) );
		}
		
		m_TopSelector		= new OptionSelectorMultistateCharacterMenu( layoutRoot.FindAnyWidget( "character_top_setting_option" ), 0, null, false, DefaultCharacterCreationMethods.GetConfigAttachmentTypes(InventorySlots.BODY) );
		m_BottomSelector	= new OptionSelectorMultistateCharacterMenu( layoutRoot.FindAnyWidget( "character_bottom_setting_option" ), 0, null, false, DefaultCharacterCreationMethods.GetConfigAttachmentTypes(InventorySlots.LEGS) );
		m_ShoesSelector		= new OptionSelectorMultistateCharacterMenu( layoutRoot.FindAnyWidget( "character_shoes_setting_option" ), 0, null, false, DefaultCharacterCreationMethods.GetConfigAttachmentTypes(InventorySlots.FEET) );
		
		PlayerBase scene_char = GetPlayerObj();
		if( scene_char )
		{
			Object obj = scene_char.GetInventory().FindAttachment(InventorySlots.BODY);
			if( obj )
				m_TopSelector.SetValue( obj.GetType(), false );
			
			obj = scene_char.GetInventory().FindAttachment(InventorySlots.LEGS);
			if( obj )
				m_BottomSelector.SetValue( obj.GetType(), false );
			
			obj = scene_char.GetInventory().FindAttachment(InventorySlots.FEET);
			if( obj )
				m_ShoesSelector.SetValue( obj.GetType(), false );
			
			m_SkinSelector.SetValue( scene_char.GetType() );
		}
		
		m_GenderSelector.m_OptionChanged.Insert( GenderChanged );
		m_SkinSelector.m_OptionChanged.Insert( SkinChanged );
		m_TopSelector.m_OptionChanged.Insert( TopChanged );
		m_BottomSelector.m_OptionChanged.Insert( BottomChanged );
		m_ShoesSelector.m_OptionChanged.Insert( ShoesChanged );
		
		#ifdef PLATFORM_CONSOLE
		UpdateControlsElements();
		#endif
		
		Refresh();
		SetCharacter();
		CheckNewOptions();
		
		Widget m_separator = layoutRoot.FindAnyWidget("separator_red");
		m_separator.SetColor( colorScheme.BrandColor() );
		
		return layoutRoot;
	}
	
	//Coloring functions (Until WidgetStyles are useful)
	void ColorHighlight( Widget w )
	{
		if( w.IsInherited( ButtonWidget ) )
		{
			ButtonWidget button = ButtonWidget.Cast( w );
			button.SetTextColor( colorScheme.BrandColor()  );
		}
		
		w.SetColor( ARGB( 255, 0, 0, 0) );
		
		TextWidget text1	= TextWidget.Cast(w.FindAnyWidget( w.GetName() + "_text" ) );
		TextWidget text2	= TextWidget.Cast(w.FindAnyWidget( w.GetName() + "_label" ) );
		TextWidget text3	= TextWidget.Cast(w.FindAnyWidget( w.GetName() + "_text_1" ) );
		ImageWidget image	= ImageWidget.Cast( w.FindAnyWidget( w.GetName() + "_image" ) );
		Widget option		= Widget.Cast( w.FindAnyWidget( w.GetName() + "_option_wrapper" ) );
		Widget option_label = w.FindAnyWidget( "option_label" );
		
		if( text1 )
		{
			text1.SetColor( colorScheme.BrandColor() );
		}
		
		if( text2 )
		{
			text2.SetColor( colorScheme.BrandColor() );
		}
		
		if( text3 )
		{
			text3.SetColor( colorScheme.BrandColor() );
			w.SetAlpha(1);
		}
		
		if( image )
		{
			image.SetColor( colorScheme.BrandColor() );
		}
		
		if ( option )
		{
			option.SetColor( colorScheme.BrandColor() );
		}
		
		#ifndef PLATFORM_CONSOLE
		if ( option_label )
		{
			option_label.SetColor( colorScheme.BrandColor() );
		}
		#endif
	}
	
	void ColorNormal( Widget w )
	{
		if( w.IsInherited( ButtonWidget ) )
		{
			ButtonWidget button = ButtonWidget.Cast( w );
			button.SetTextColor( UIColor.White() );
		}
		
		TextWidget text1	= TextWidget.Cast(w.FindAnyWidget( w.GetName() + "_text" ) );
		TextWidget text2	= TextWidget.Cast(w.FindAnyWidget( w.GetName() + "_text_1" ) );
		TextWidget text3	= TextWidget.Cast(w.FindAnyWidget( w.GetName() + "_label" ) );
		ImageWidget image	= ImageWidget.Cast( w.FindAnyWidget( w.GetName() + "_image" ) );
		Widget option		= w.FindAnyWidget( w.GetName() + "_option_wrapper" );
		Widget option_label = w.FindAnyWidget( "option_label" );
		
		if( text1 )
		{
			text1.SetColor( UIColor.White() );
		}
		
		if( text2 )
		{
			text2.SetColor( UIColor.White() );
		}
		
		if( text3 )
		{
			text3.SetColor( UIColor.White() );
			w.SetAlpha(0);
		}
		
		if( image )
		{
			image.SetColor( UIColor.White() );
		}
		
		if ( option )
		{
			option.SetColor( UIColor.White() );
		}
		
		#ifndef PLATFORM_CONSOLE
		if ( option_label )
		{
			option_label.SetColor( UIColor.White() );
		}
		#endif
	}
	
	void ColorDisable( Widget w )
	{
		#ifndef PLATFORM_CONSOLE
		SetFocus( null );
		#endif
		
		if ( w )
		{
			ButtonWidget button = ButtonWidget.Cast( w );
			if( button )
			{
				button.SetTextColor( ColorManager.COLOR_DISABLED_TEXT );
			}
		}
	}
	
}