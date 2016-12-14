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
    [1, 18] => 104, # lightbulb + crowbar => can't combine
    [1, 19] => 104, # lightbulb + pipe => can't combine
    [25, 26] => 30, # feather + ink bottle => pen
    [27, 56] => 57, # tube of water + water pill => miracle water
    [32, 36] => 33, # button + magnets => magnetized button
    [37, 38] => 32, # tin + scissors => button
    [36, 37] => 100, # tin + magnets => can't combine
    [44, 46] => 52, # gluestick + Kip's library card => Kip's sticky library card
    [46, 53] => 51, # Kip's library card + sticky photo of Niko => Niko's library card
    [44, 45] => 53, # gluestick + photo of Niko => sticky photo of Niko
    [44, 61] => 71, # gluestick + photo of Niko1 => sticky photo of Niko
    [44, 62] => 72, # gluestick + photo of Niko2 => sticky photo of Niko
    [44, 63] => 73, # gluestick + photo of Niko3 => sticky photo of Niko
    [44, 64] => 74, # gluestick + photo of Niko4 => sticky photo of Niko
    [44, 65] => 75, # gluestick + photo of Niko5 => sticky photo of Niko
    [44, 66] => 54, # gluestick + photo of Niko6 'blink' => can't combine
    [44, 67] => 77, # gluestick + photo of Niko7 => sticky photo of Niko
    [44, 68] => 78, # gluestick + photo of Niko8 => sticky photo of Niko
    [44, 69] => 79, # gluestick + photo of Niko9 => sticky photo of Niko
    [44, 70] => 80, # gluestick + photo of Niko10 => sticky photo of Niko
    [45, 52] => 51, # photo of Niko + Kip's sticky library card => Niko's library card
    [52, 61] => 61, # photo of Niko1 + Kip's sticky library card => Niko's library card
    [52, 62] => 62, # photo of Niko2 + Kip's sticky library card => Niko's library card
    [52, 63] => 63, # photo of Niko3 + Kip's sticky library card => Niko's library card
    [52, 64] => 64, # photo of Niko4 + Kip's sticky library card => Niko's library card
    [52, 65] => 65, # photo of Niko5 + Kip's sticky library card => Niko's library card
    [52, 66] => 54, # photo of Niko6 'blink' + Kip's sticky library card => can't combine
    [52, 67] => 67, # photo of Niko7 + Kip's sticky library card => Niko's library card
    [52, 68] => 68, # photo of Niko8 + Kip's sticky library card => Niko's library card
    [52, 69] => 69, # photo of Niko9 + Kip's sticky library card => Niko's library card
    [52, 70] => 70, # photo of Niko10 + Kip's sticky library card => Niko's library card
  }

  def self.combine(item_a, item_b)
    items = [item_a, item_b].minmax
    return COMBINATIONS[items]
  end
end
