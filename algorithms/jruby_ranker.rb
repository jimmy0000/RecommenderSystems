require 'java'

class Ranker
  def initialize(options={})
    options[:output_file] = "jruby_ranker_output.log" unless options.has_key?(:output_file)
    hashMap = java.util.HashMap.new
    options.each do |key, value|
      hashMap.put(key.to_s, value)
    end
    @java_ranker = Java::Ranker.new(hashMap)
  end

  def launch!
    @java_ranker.launch
  end
end

#ranker = Ranker.new({train_file: "sample_train.txt", validation_file: "sample_validation.txt", test_file: "sample_publictest.txt"})
ranker = Ranker.new({train_file: "extraction_train.txt", validation_file: "extraction_validation.txt", test_file: "extraction_publictest.txt"})
ranker.launch!

