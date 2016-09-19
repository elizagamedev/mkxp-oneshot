DOCUMENT_TEXT = \
"%06d"

def safe_puzzle_write
  File.open(Oneshot::DOCS_PATH + "/INFORMATION.oneshot.txt", 'w') do |file|
    file.puts(DOCUMENT_TEXT % $game_variables[20])
  end
end
