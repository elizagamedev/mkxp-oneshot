# Classes for translating text similar to GNU gettext

# Translation class: contains translation data
class Translation
  attr_reader :events
  attr_reader :scripts
  attr_reader :actors
  attr_reader :items
end

# Translator class: translate text to another language
class Translator
  # Create Translator object
  def initialize(lc)
    @data = nil
    [lc.full.to_s, lc.lang.to_s].each do |name|
      begin
        @data = load_data("Langauges/#{name}.rxdata")
        break
      rescue
      end
    end
  end

  # Translate all database items
  def translate_database
    $data_actors.each do |i|
      i.name = actor(i.name) if i && !i.name.empty?
    end
    $data_items.each do |i|
      i.name, i.description = item(i.name, i.description) if i && !i.name.empty?
    end
  end

  # Translate script text
  def script(name, text)
    if @data == nil
      return text
    else
      return @data.scripts["#{name}/#{text}"] || text
    end
  end

  # Translate text from event
  def event(name, text)
    if @data == nil
      return text
    else
      return @data.events["#{name}/#{text}"] || text
    end
  end

  private
  # Translate Actor name
  def actor(name)
    if @data == nil
      return name
    else
      return @data.actors[name] || name
    end
  end

  # Translate Item name (and description)
  def item(name, description)
    if @data == nil
      return [name, description]
    else
      return @data.items[name] || [name, description]
    end
  end
end

# Translatable string
class TrString
  def initialize(str)
    @str = str
  end

  def to_str
    @str
  end
  alias :to_s :to_str
end

def tr(text)
  TrString.new(text)
end
