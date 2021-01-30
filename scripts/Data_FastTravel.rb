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
    :red_ground => Zone.new(tr("The Refuge (Surface)"), {
      :ground1 => tr("elevator street"),
      :ground2 => tr("vendor street"),
      :backalley => tr("back alley"),
      :library => tr("library"),
      :factory => tr("factory"),
    }),
    :red => Zone.new(tr("The Refuge"), {
      :garden => tr("garden"),
      :gate => tr("city gate"),
      :elevator => tr("elevator deck"),
      :apartments => tr("apartments"),
      :cafe => tr("cafe"),
      :office => tr("office"),
      :obsdeck => tr("observation deck"),
    }),
    :green => Zone.new(tr("The Glen"), {
      :village => tr("village"),
      :ruins => tr("ruins"),
      :forest => tr("forest"),
      :wall => tr("the gate"),
      :dock => tr("dock"),
      :courtyard => tr("courtyard"),
	  :research => tr("research station"),
	  :grave => tr("graveyard")
    }),
    :blue => Zone.new(tr("The Barrens"), {
      :entrance => tr("entrance"),
      :outpost => tr("outpost"),
      :cliffs => tr("cliffs"),
      :mineshaft => tr("mineshaft entrance"),
      :factory => tr("old factory"),
      :dorms => tr("dormitories"),
      :swamp => tr("shrimp swamp"),
      :docks => tr("docks"),
      :quarry => tr("lookout point"),
    }),
  }
end
