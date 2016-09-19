module EdText
  def self.blue_understand
    Oneshot.msgbox Oneshot::Msg::YESNO, tr("Do you understand what this means?")
  end
end
