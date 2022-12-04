#==============================================================================
# ** Game_Temp
#------------------------------------------------------------------------------
#  This class handles temporary data that is not included with save data.
#  Refer to "$game_temp" for the instance of this class.
#==============================================================================

class Game_Temp
  #--------------------------------------------------------------------------
  # * Public Instance Variables
  #--------------------------------------------------------------------------
  attr_accessor :target_bgm_vol_level     # volume we want to reach when fading in music
  attr_accessor :bgm_fadein_speed         # speed at which we will reach target bgm vol_level
  attr_accessor :bgm_fadein_timer         # used so we aren't changing the volume every frame
  attr_accessor :map_bgm                  # map music (for battle memory)
  attr_accessor :message_text             # message text
  attr_accessor :message_face             # face graphic
  attr_accessor :message_proc             # message callback (Proc)
  attr_accessor :message_ed_text          # Ed message text
  attr_accessor :message_doc_text         # Text message text
  attr_accessor :message_desktop_text     # Desktop message text
  attr_accessor :message_credits_text     # Desktop message text
  attr_accessor :choices                  # show choices: text
  attr_accessor :choice_cancel_type       # show choices: cancel
  attr_accessor :choice_proc              # show choices: callback (Proc)
  attr_accessor :num_input_variable_id    # input number: variable ID
  attr_accessor :num_input_digits_max     # input number: digit amount
  attr_accessor :message_window_showing   # message window showing
  attr_accessor :common_event_id          # common event ID
  attr_accessor :in_battle                # in-battle flag
  attr_accessor :battle_calling           # battle calling flag
  attr_accessor :battle_troop_id          # battle troop ID
  attr_accessor :battle_can_escape        # battle flag: escape possible
  attr_accessor :battle_can_lose          # battle flag: losing possible
  attr_accessor :battle_proc              # battle callback (Proc)
  attr_accessor :battle_turn              # number of battle turns
  attr_accessor :battle_event_flags       # battle event flags: completed
  attr_accessor :battle_abort             # battle flag: interrupt
  attr_accessor :battle_main_phase        # battle flag: main phase
  attr_accessor :battleback_name          # battleback file name
  attr_accessor :forcing_battler          # battler being forced into action
  attr_accessor :shop_calling             # shop calling flag
  attr_accessor :shop_goods               # list of shop goods
  attr_accessor :name_calling             # name input: calling flag
  attr_accessor :menu_calling             # menu calling flag
  attr_accessor :item_menu_calling        # item menu calling flag
  attr_accessor :travel_menu_calling      # fast travel menu calling flag
  attr_accessor :window_settings_calling      # fast travel menu calling flag
  attr_accessor :menu_beep                # menu: play sound effect flag
  attr_accessor :save_calling             # save calling flag
  attr_accessor :debug_calling            # debug calling flag
  attr_accessor :player_transferring      # player place movement flag
  attr_accessor :player_new_map_id        # player destination: map ID
  attr_accessor :player_new_x             # player destination: x-coordinate
  attr_accessor :player_new_y             # player destination: y-coordinate
  attr_accessor :player_new_direction     # player destination: direction
  attr_accessor :transition_processing    # transition processing flag
  attr_accessor :transition_name          # transition file name
  attr_accessor :gameover                 # game over flag
  attr_accessor :to_title                 # return to title screen flag
  attr_accessor :last_file_index          # last save file no.
  attr_accessor :debug_top_row            # debug screen: for saving conditions
  attr_accessor :debug_index              # debug screen: for saving conditions
  attr_accessor :footstep_sfx             # current footstep sfx array
  attr_accessor :filmsprite               # film puzzle sprite
  attr_accessor :prompt_wait              # wait for delay caused by prompt
  attr_accessor :menus_visible
  attr_accessor :countdown_password
  attr_accessor :igt_timer_visible
  #--------------------------------------------------------------------------
  # * Object Initialization
  #--------------------------------------------------------------------------
  def initialize
    @target_bgm_vol_level = -1
    @bgm_fadein_speed = 0
    @bgm_fadein_timer = 0
    @map_bgm = nil
    @message_text = nil
    @message_face = nil
    @message_proc = nil
    @choices = nil
    @choice_cancel_type = 0
    @choice_proc = nil
    @num_input_variable_id = 0
    @num_input_digits_max = 0
    @message_window_showing = false
    @common_event_id = 0
    @in_battle = false
    @battle_calling = false
    @battle_troop_id = 0
    @battle_can_escape = false
    @battle_can_lose = false
    @battle_proc = nil
    @battle_turn = 0
    @battle_event_flags = {}
    @battle_abort = false
    @battle_main_phase = false
    @battleback_name = ''
    @forcing_battler = nil
    @shop_calling = false
    @shop_id = 0
    @name_calling = false
    @name_actor_id = 0
    @name_max_char = 0
    @menu_calling = false
    @item_menu_calling = false
    @travel_menu_calling = false
    @window_settings_calling = false
    @menu_beep = false
    @save_calling = false
    @debug_calling = false
    @player_transferring = false
    @player_new_map_id = 0
    @player_new_x = 0
    @player_new_y = 0
    @player_new_direction = 0
    @transition_processing = false
    @transition_name = ""
    @gameover = false
    @to_title = false
    @last_file_index = 0
    @debug_top_row = 0
    @debug_index = 0
    @footstep_sfx = nil
    @filmsprite = nil
    @prompt_wait = 0
    @menus_visible = false
    @countdown_password = ""
    @igt_timer_visible = false
  end

  def bgm_fadein(game_system)
    if @target_bgm_vol_level > 0
      @bgm_fadein_timer += 1
      if @bgm_fadein_timer > 14
        @bgm_fadein_timer = 0
        game_system.playing_bgm.volume += @bgm_fadein_speed
        if game_system.playing_bgm.volume >= @target_bgm_vol_level
          game_system.playing_bgm.volume = @target_bgm_vol_level
          @target_bgm_vol_level = -1
          @bgm_fadein_speed = 1
        end
        game_system.bgm_play(game_system.playing_bgm)
      end
    end
  end
end
