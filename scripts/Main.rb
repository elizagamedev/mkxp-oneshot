#==============================================================================
# ** Main
#------------------------------------------------------------------------------
#  After defining each class, actual processing begins here.
#==============================================================================

at_exit do
  save unless ($game_system.map_interpreter.running? || !$scene.is_a?(Scene_Map))
end

begin
  $console = Graphics.fullscreen
  Graphics.frame_rate = 60
  Font.default_size = 20

  # Load persistent data
  Persistent.load
  $persistent.lang = 'ja'

  # Prepare for transition
  Graphics.freeze
  $debug = false
  $demo = false
  $GDC = false
  # Make scene object (title screen)
  $scene = Scene_Title.new
  # Call main method as long as $scene is effective
  while $scene != nil
    $scene.main
  end
  # Fade out
  Graphics.transition(20)
rescue Errno::ENOENT
  # Supplement Errno::ENOENT exception
  # If unable to open file, display message and end
  filename = $!.message.sub("No such file or directory - ", "")
  print("Unable to find file #{filename}.")
end
