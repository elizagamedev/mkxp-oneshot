module Item
  COMBINATIONS = {
    [3, 4] => 5, # alcohol + dry branch => wet branch
    [8, 9] => 10, # camera + screwdriver => lens
    [10, 12] => 13, # lens + broken battery => empty battery
    [1, 13] => 14, # lightbulb + empty battery => charged battery
    [1, 12] => 102, # lightbulb + broken battery => can't combine
    [16, 20] => 17, # sponge + bottle of acid => wet sponge
    [11, 22] => 101, # empty bottle + viscous syringe => can't combine
    [15, 22] => 16, # bottle of smoke + viscous syringe => bottle of acid
    [11, 18] => 103, # empty bottle + crowbar => can't combine
    [11, 19] => 103, # empty bottle + pipe => can't combine
    [25, 26] => 30, # feather + ink bottle => pen
    [27, 56] => 57, # tube of water + water pill => miracle water
    [32, 36] => 33, # button + magnets => magnetized button
    [37, 38] => 32, # tin + scissors => button
    [36, 37] => 100, # tin + magnets => can't combine
    [44, 45] => 53, # gluestick + photo of Niko => sticky photo of Niko
    [44, 46] => 52, # gluestick + Kip's library card => Kip's sticky library card
    [45, 52] => 51, # photo of Niko + Kip's sticky library card => Niko's library card
    [46, 53] => 51, # Kip's library card + sticky photo of Niko => Niko's library card
  }

  def self.combine(item_a, item_b)
    items = [item_a, item_b].minmax
    return COMBINATIONS[items]
  end
end
