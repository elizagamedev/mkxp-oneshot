#==============================================================================
# ** Window_Help
#------------------------------------------------------------------------------
#  This window shows skill and item explanations along with actor status.
#==============================================================================

class Window_Help < Window_Base
  #--------------------------------------------------------------------------
  # * Object Initialization
  #--------------------------------------------------------------------------
  def initialize
    super(16, 16, 608, 64)
    self.contents = Bitmap.new(width - 32, height - 32)
    Language.register_text_sprite(self.class.name + "_contents", self.contents)
    self.visible = false
    self.back_opacity = 230
    self.z = 9998
  end
  #--------------------------------------------------------------------------
  # * Set Text
  #  text  : text string displayed in window
  #  align : alignment (0..flush left, 1..center, 2..flush right)
  #--------------------------------------------------------------------------
  def set_text(text, align = 0)
    # Redraw text
    self.contents.clear
    self.contents.font.color = normal_color
    self.contents.draw_text(4, 0, self.width - 40, 32, text, align)
    @text = text
    @align = align
    @actor = nil
  end
end
