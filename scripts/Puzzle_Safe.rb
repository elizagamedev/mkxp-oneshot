
def safe_puzzle_write
  docTextFilePath = "Languages/safetext/#{$persistent.langcode}/safe1.txt"
  
  #use default file (english) if we can't find the language specific on
  if !FileTest.exist?(docTextFilePath)
    docTextFilePath = "Languages/safetext/safe1.txt"
  end
  doc_text = File.read(docTextFilePath)
  
  Dir.mkdir(Oneshot::DOCS_PATH) unless File.exists?(Oneshot::DOCS_PATH)
  File.open("#{Oneshot::DOCS_PATH}/DOCUMENT.oneshot.txt", 'w') do |file|
    file.puts(doc_text + $game_variables[20].to_s)
  end
end

def safe_puzzle_postgame_write
  docTextFilePath = "Languages/safetext/#{$persistent.langcode}/safe2.txt"
  
  #use default file (english) if we can't find the language specific on
  if !FileTest.exist?(docTextFilePath)
    docTextFilePath = "Languages/safetext/safe2.txt"
  end
  doc_text = File.read(docTextFilePath)
  
  Dir.mkdir(Oneshot::DOCS_PATH) unless File.exists?(Oneshot::DOCS_PATH)
  File.open("#{Oneshot::DOCS_PATH}/DOCUMENT.oneshot.txt", 'w') do |file|
    file.puts(doc_text + $game_variables[20].to_s)
  end
end
