set(scripts
  Credits_Message.rb
  Data_FastTravel.rb
  Data_Footsteps.rb
  Data_Item.rb
  Data_SpecialEventData.rb
  Data_TileText.rb
  Demo.rb
  Desktop_Message.rb
  Doc_Message.rb
  EdText.rb
  Ed_Message.rb
  FastTravel.rb
  Game_Actor.rb
  Game_Actors.rb
  Game_BattleAction.rb
  Game_Battler 1.rb
  Game_Battler 2.rb
  Game_Battler 3.rb
  Game_Character 1.rb
  Game_Character 2.rb
  Game_Character 3.rb
  Game_CommonEvent.rb
  Game_Enemy.rb
  Game_Event.rb
  Game_FastTravel.rb
  Game_Follower.rb
  Game_Light.rb
  Game_Map.rb
  Game_Oneshot.rb
  Game_Party.rb
  Game_Picture.rb
  Game_Player.rb
  Game_Screen.rb
  Game_SelfSwitches.rb
  Game_Switches.rb
  Game_System.rb
  Game_Temp.rb
  Game_Variables.rb
  Interpreter 1.rb
  Interpreter 2.rb
  Interpreter 3.rb
  Interpreter 4.rb
  Interpreter 5.rb
  Interpreter 6.rb
  Interpreter 7.rb
  Main.rb
  Particles.rb
  Persistent.rb
  Puzzle_Film.rb
  Puzzle_Pixel.rb
  Puzzle_Safe.rb
  Puzzle_Sokoban.rb
  RPG.rb
  SaveLoad.rb
  Scene_Debug.rb
  Scene_End.rb
  Scene_Equip.rb
  Scene_File.rb
  Scene_Item.rb
  Scene_Load.rb
  Scene_Map.rb
  Scene_Menu.rb
  Scene_Name.rb
  Scene_Save.rb
  Scene_Skill.rb
  Scene_Status.rb
  Scene_Title.rb
  Script.rb
  Sprite_Battler.rb
  Sprite_Character.rb
  Sprite_Footprint.rb
  Sprite_Footsplash.rb
  Sprite_Light.rb
  Sprite_MapText.rb
  Sprite_Picture.rb
  Sprite_Timer.rb
  Spriteset_Map.rb
  Window_Base.rb
  Window_BattleResult.rb
  Window_BattleStatus.rb
  Window_Command.rb
  Window_DebugLeft.rb
  Window_DebugRight.rb
  Window_EquipItem.rb
  Window_EquipLeft.rb
  Window_EquipRight.rb
  Window_Gold.rb
  Window_Help.rb
  Window_InputNumber.rb
  Window_Item.rb
  Window_MainMenu.rb
  Window_MenuStatus.rb
  Window_Message.rb
  Window_NameEdit.rb
  Window_NameInput.rb
  Window_PartyCommand.rb
  Window_PlayTime.rb
  Window_SaveFile.rb
  Window_Selectable.rb
  Window_Settings.rb
  Window_ShopBuy.rb
  Window_ShopCommand.rb
  Window_ShopNumber.rb
  Window_ShopSell.rb
  Window_ShopStatus.rb
  Window_Skill.rb
  Window_SkillStatus.rb
  Window_Status.rb
  Window_Steps.rb
  Window_Target.rb
  i18n_English.rb
  i18n_Japanese.rb
  i18n_Language.rb)

find_program(RUBY_EXE ruby
  DOC "Location of the Ruby executable")
set(game_dir "${CMAKE_BINARY_DIR}/bin")
set(xscripts "${game_dir}/Data/xScripts.rxdata")
add_custom_command(
  OUTPUT "${xscripts}"
  COMMAND "${RUBY_EXE}" "${CMAKE_SOURCE_DIR}/rpgscript.rb" "${CMAKE_CURRENT_LIST_DIR}" "${game_dir}"
  DEPENDS _scripts.txt ${scripts}
  COMMENT "Bundling xScripts.rxdata")
add_custom_target(xScripts ALL DEPENDS "${xscripts}")
