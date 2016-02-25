class FastTravel
  class Zone
    attr_reader :name
    attr_reader :maps
    def initialize(name, maps)
      @name = name
      @maps = maps
    end
  end

  ZONES = {
    :red => Zone.new(tr("The Refuge"), {
      :gate => tr("city gate"),
      :elevator => tr("elevator deck"),
      :apartments => tr("apartments"),
      :cafe => tr("cafe"),
      :office => tr("office"),
    }),
    :green => Zone.new(tr("The Glen"), {
      :whatever => tr(""),
    }),
    :blue => Zone.new(tr("The Barrens"), {
      :entrance => tr("entrance"),
      :outpost => tr("outpost"),
      :cliffs => tr("cliffs"),
      :quarry => tr("quarry"),
      :mineshaft => tr("mineshaft entrance"),
      :factory => tr("old factory"),
      :dorms => tr("dormitories"),
      :swamp => tr("shrimp swamp"),
      :docks => tr("docks"),
    }),
  }
end
