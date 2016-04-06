# Default English localization settings
Font.default_name = Language::FONT_WESTERN

class Language
  def self.create_input
    input = Window_NameInput.new
    input.set_mode_buttons [['Upper', 'Lower']]
    input.character_tables = {
      [0] => [ # Case: Upper
        "A","B","C","D","E","F","G","H","I","J",
        "K","L","M","N","O","P","Q","R","S","T",
        "U","V","W","X","Y","Z"," ",".",",","-",
        "0","1","2","3","4","5","6","7","8","9",
        "!","?",'"',"'","/","&","[","]","(",")",
      ],
      [1] => [ # Case: Lower
        "a","b","c","d","e","f","g","h","i","j",
        "k","l","m","n","o","p","q","r","s","t",
        "u","v","w","x","y","z"," ",".",",","-",
        "0","1","2","3","4","5","6","7","8","9",
        "!","?",'"',"'","/","&","[","]","(",")",
      ],
    }
    input.table_height = 5
    input.init
  end
end
