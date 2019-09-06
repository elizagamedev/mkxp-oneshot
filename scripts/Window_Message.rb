#==============================================================================
# ** Window_Message
#------------------------------------------------------------------------------
#  This message window is used to display text.
#==============================================================================

class Window_Message < Window_Selectable
  BLIP_TIME = 4
  #--------------------------------------------------------------------------
  # * Object Initialization
  #--------------------------------------------------------------------------
  def initialize
    super(16, 336, 608, 128)
    self.contents = Bitmap.new(width - 32, height - 32)
    Language.register_text_sprite(self.class.name + "_contents", self.contents)
    self.visible = false
    self.z = 9999
    self.back_opacity = 210

    # Animation flags
    @fade_in = false
    @fade_out = false
    @opaque = true # set to false if we're not rendering the window bg

    @text_pause = 0
    @blip = 0

    # Text drawing flags
    @text = nil
    @text_y = @text_x = 0
    @drawing_text = false # set to true when message text is drawing
    @skip_text = false

    # Number/Choices
    self.active = false
    self.index = -1
    @choice_start = -1
    @number_start = -1
    # skip message proc if we have choices/numbers buffered
    @skip_message_proc = false

    # Text blip sound
    @blipsound = nil
  end
  #--------------------------------------------------------------------------
  # * Dispose
  #--------------------------------------------------------------------------
  def dispose
    terminate_message
    $game_temp.message_window_showing = false
    if @input_number_window != nil
      @input_number_window.dispose
    end
    super
  end
  #--------------------------------------------------------------------------
  # * Terminate Message
  #--------------------------------------------------------------------------
  def terminate_message
    # Call message callback
    if !@skip_message_proc && $game_temp.message_proc != nil
      $game_temp.message_proc.call
      $game_temp.message_proc = nil
    end
    # Clear variables related to text, choices, and number input
    $game_temp.message_text = nil
    $game_temp.message_face = nil
    if @choice_start >= 0
      $game_temp.choices = nil
      $game_temp.choice_cancel_type = 0
      $game_temp.choice_proc = nil
    elsif @number_start >= 0
      $game_temp.num_input_variable_id = 0
      $game_temp.num_input_digits_max = 0
    end
    # Reset state
    @text = nil
    @choice_start = -1
    @number_start = -1
    @skip_message_proc = false
    self.active = false
    self.pause = false
    self.index = -1
  end
  #--------------------------------------------------------------------------
  # * Refresh: Load new message text and pre-process it
  #--------------------------------------------------------------------------
  def refresh
    # Initialize
    @blip = BLIP_TIME
    @text = ''

    # Pre-process text
    if $game_temp.message_text != nil && !$game_temp.message_text.empty?
      text = $game_temp.message_text

      # Determine blip sound
      @blipsound = text.start_with?('[') ? 'text_robot' : 'text'

      # Substitute variables, actors, player name, newlines, etc
      text.gsub!(/\\v\[([0-9]+)\]/) do
        $game_variables[$1.to_i]
      end
	  #add a space to the beginning of the player name to better deal with longer names in asian languages
	  if (Language::FONT_WESTERN == Font.default_name)
        text.gsub!("\\p", $game_oneshot.player_name)
      else
        text.gsub!("\\p", " " + $game_oneshot.player_name)
      end
      text.gsub!("\\n", "\n")
      # Handle text-rendering escape sequences
      text.gsub!(/\\c\[([0-9]+)\]/, "\0[\\1]")
      text.gsub!("\\.", "\001")
      text.gsub!("\\|", "\002")
      text.gsub!("\\>", "\004")


      text.gsub!("\\@", "\003")
      # Finally convert the backslash back
      text.gsub!("\\\\", "\\")

      # Now split text into lines by measuring text metrics
      x = y = 0
      maxwidth = self.contents.width - 4 - ($game_temp.message_face == nil ? 0 : 96)
      spacewidth = self.contents.text_size(' ').width
      text.split("\n").each do |line|
        line.split(' ').each do |word|

          # Get width of this word and insert a newline if it goes out of bounds
          width = self.contents.text_size(word.gsub(/(\000\[[0-9]+\]|\001|\002)/, '')).width
          if word.include?("\003")
            #ignore new facepics
            width = 0
          end
          if x + width > maxwidth
            @text << "\n"
            x = 0
            y += 1
            break if y >= 4
          end

          # Append word to list
          if x == 0
            @text << word
          else
            @text << ' ' << word
          end
          x += width + spacewidth
        end

        # Newline
        @text << "\n"
        x = 0
        y += 1
        break if y >= 4
      end
    end
    @text.rstrip!
    lines = @text.empty? ? 0 : @text.count("\n") + 1

    # Prepare renderer
    self.contents.clear
    self.contents.font.color = normal_color
    @text_y = @text_x = 0

    # Blit face graphic
    if $game_temp.message_face != nil
      if $game_player.character_name == "niko_gasmask" || $game_player.character_name == "niko_bulb_gasmask" \
		|| $game_player.character_name == "en_gasmask" || $game_player.character_name == "en_bulb_gasmask"
        if $game_temp.message_face.start_with?("niko")
          $game_temp.message_face = "niko_gasmask"
        end
		if $game_temp.message_face.start_with?("en")
          $game_temp.message_face = "en_gasmask"
        end
      end
      face = RPG::Cache.face($game_temp.message_face)
      self.contents.blt(self.contents.width - 96, 0, face, Rect.new(0, 0, 96, 96))
    end

    if $game_temp.choices != nil
      # Prepare choices, if they fit
      if lines + $game_temp.choices.size <= 4
        @choice_start = lines
        @item_max = $game_temp.choices.size
      else
        # Don't call the message callback till we can show all the choices
        @skip_message_proc = true
      end
    elsif $game_temp.num_input_variable_id > 0
      # Prepare number input, if it fits
      if lines < 3
        @number_start = lines
      else
        # Don't call the message callback till we get a number
        @skip_message_proc = true
      end
    end
  end
  #--------------------------------------------------------------------------
  # * Tick: render a new character to the message box
  #--------------------------------------------------------------------------
  def tick
    return if @text_pause > 0
    # Don't do anything if we're done
    return if !@drawing_text

    # Get 1 text character in c (loop until unable to get text)
    while ((c = @text.slice!(0)) != nil)
      # \n
      if c == "\n"
        @text_x = 0
        @text_y += 1
        next
      end
      # \c[n]
      if c == "\000"
        # Change text color
        @text.sub!(/\[([0-9]+)\]/, "")
        color = $1.to_i
        if color >= 0 and color <= 7
          self.contents.font.color = Window_Base.text_color(color)
        end
        # go to next text
        next
      end
      # \.
      if c == "\001"
        # Pause
        @text_pause = 10
        return
      end
      # \|
      if c == "\002"
        # Pause
        @text_pause = 10*4
        return
      end
      if c == "\003"
        # new facepic
        face_name = @text.slice!(/^[^\s]+ */).strip()
        self.contents.fill_rect(self.contents.width - 96, 0, 96, 96, Color.new(0,0,0,0))
        $game_temp.message_face = face_name
		if $game_player.character_name == "niko_gasmask" || $game_player.character_name == "niko_bulb_gasmask" \
      	  || $game_player.character_name == "en_gasmask" || $game_player.character_name == "en_bulb_gasmask"
          if $game_temp.message_face.start_with?("niko")
            $game_temp.message_face = "niko_gasmask"
          end
		  if $game_temp.message_face.start_with?("en")
            $game_temp.message_face = "en_gasmask"
          end
        end
        face = RPG::Cache.face($game_temp.message_face)
        self.contents.blt(self.contents.width - 96, 0, face, Rect.new(0, 0, 96, 96))
        return
      end
      if c == "\004"
        @drawing_text = false
        @skip_text = false
        return
      end
      # Draw text
      self.contents.draw_text(4 + @text_x, 24 * @text_y, 40, 24, c)
      # Add x to drawn text width
      @text_x += self.contents.text_size(c).width
      if !@skip_text
        break
      end
    end
    @skip_text = false

    # If text is empty, set up choices/numbers and indicate that we're done
    if @text.empty?
      @drawing_text = false

      if @choice_start >= 0
        # Setup and draw choices
        self.index = 0
        self.active = true
        self.contents.font.color = normal_color
        $game_temp.choices.each_with_index do |choice, i|
          if choice != nil
            choice.gsub!("\\p", $game_oneshot.player_name)
          end
          self.contents.draw_text(12, 24 * (@choice_start + i), self.contents.width, 24, choice)
        end
      elsif @number_start >= 0
        # Setup numbers
        digits_max = $game_temp.num_input_digits_max
        number = $game_variables[$game_temp.num_input_variable_id]
        @input_number_window = Window_InputNumber.new(digits_max)
        @input_number_window.number = number
        @input_number_window.x = self.x + 8
        @input_number_window.y = self.y + @number_start * 24
      end
    end
  end
  #--------------------------------------------------------------------------
  # * Set Window Position and Opacity Level
  #--------------------------------------------------------------------------
  def reset_window
    if $game_temp.in_battle
      self.y = 16
    else
      case $game_system.message_position
      when 0  # up
        self.y = 16
      when 1  # middle
        self.y = 160
      when 2  # down
        self.y = 336
      end
    end
    if $game_system.message_frame == 0
      @opaque = true
    else
      @opaque = false
    end
  end
  #--------------------------------------------------------------------------
  # * Frame Update
  #--------------------------------------------------------------------------
  def update
    super

    # Handle fade-out effect
    if @fade_out
      self.opacity -= 48
      self.contents_opacity -= 48*2
      if self.opacity == 0
        @fade_out = false
        self.visible = false
        self.contents.clear
        $game_temp.message_window_showing = false
      end
      return
    end

    # Handle fade-in effect
    if @fade_in
      self.opacity += 48 if @opaque
      self.contents_opacity += 48
      if @input_number_window != nil
        @input_number_window.contents_opacity += 48
      end
      if self.contents_opacity == 255
        @fade_in = false
        @drawing_text = true
        $game_temp.message_window_showing = true
      end
      return
    end

    # Message is over and should be hidden or advanced to next
    if @text == nil
      if $game_temp.message_text == nil && $game_temp.choices == nil && $game_temp.num_input_digits_max == 0
        @fade_out = true if self.visible
      else
        reset_window
        refresh
        Graphics.frame_reset

        if self.visible
          # Continue drawing text
          @drawing_text = true
        else
          # Fade in
          self.visible = true
          self.opacity = 0
          self.contents_opacity = 0
          if @input_number_window != nil
            @input_number_window.contents_opacity = 0
          end
          @fade_in = true
        end
      end
      return
    end

    # Update message text
    if @drawing_text
      if @text_pause > 0
        @text_pause -= 1
      else
        if @blip >= BLIP_TIME
		  #april fools
		  t = Time.now
	      if $game_temp.message_face != nil && t.month == 4 && t.day == 1 && $game_temp.message_face.start_with?("niko")
		    niko_sounds = [ "cat_2"]
			@blipsound = niko_sounds[rand(niko_sounds.length)]
            Audio.se_play("Audio/SE/#{@blipsound}.wav", 50, rand(100..125)) unless @text.empty?
	      else
            Audio.se_play("Audio/SE/#{@blipsound}.wav", 50) unless @text.empty?
		  end
          @blip = 0
        else
          @blip += 1
        end
        tick
      end
      if Input.trigger?(Input::ACTION) || Input.trigger?(Input::CANCEL) || (Input.press?(Input::R) && $game_switches[253])
        @skip_text = true
      end
    else
      # Handle user input
      if @choice_start >= 0
        # Cancel
        if Input.trigger?(Input::CANCEL) && $game_temp.choice_cancel_type > 0
          $game_system.se_play($data_system.cancel_se)
          $game_temp.choice_proc.call($game_temp.choice_cancel_type - 1)
          terminate_message
        elsif Input.trigger?(Input::ACTION)
          $game_system.se_play($data_system.decision_se)
          $game_temp.choice_proc.call(self.index)
          terminate_message
        end
      elsif @number_start >= 0
        @input_number_window.update

        # Confirm
        if Input.trigger?(Input::ACTION)
          $game_system.se_play($data_system.decision_se)
          $game_variables[$game_temp.num_input_variable_id] =
            @input_number_window.number
          $game_map.need_refresh = true
          # Dispose of number input window
          @input_number_window.dispose
          @input_number_window = nil
          terminate_message
        end
        return
      else
        # Show pause/continue sign
        self.pause = true

        # Advance/Close message
        if Input.trigger?(Input::ACTION) || Input.trigger?(Input::CANCEL) || (Input.press?(Input::R) && $game_switches[253])
          if @text.length <= 0
            terminate_message
          else
            @drawing_text = true
          end
        end
      end
    end
  end
  #--------------------------------------------------------------------------
  # * Cursor Rectangle Update
  #--------------------------------------------------------------------------
  def update_cursor_rect
    if @index >= 0
      n = @choice_start + @index
      width = self.contents.width - 8 - ($game_temp.message_face == nil ? 0 : 96)
      self.cursor_rect.set(4, n * 24, width, 24)
    else
      self.cursor_rect.empty
    end
  end
end
