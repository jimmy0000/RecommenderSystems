class Ranker
  @@MIN_ITERATIONS = 80000000
  @@MAX_ITERATIONS = 100000000
  @@NUM_ITERATIONS_FOR_TIMING = 1000000
  @@NUM_ITERATIONS_FOR_PRINT = 5000000
  @@CONVERGENCE_THRESHOLD = 1e-4
  @@STEP_SIZE = 1e-8
  @@REGULARIZATION_CONSTANT = 1e-3
  
  def initialize(options={})
    @num_features = options[:num_features]
    @train_file = options[:train_file]
    @validation_file = options[:validation_file]
    @test_file = options[:test_file]
    @output_file = File.open(options[:output_file] || "ranker_output.log", 'w')
  end

  def launch!
    begin
      @num_features ||= get_number_of_features(@train_file)
      train @train_file, @num_features
      
      test @train_file, @num_features
      test @validation_file, @num_features
      test @test_file, @num_features
    ensure
      @output_file.close unless @output_file.nil?
    end
  end

  private

    def get_number_of_features(train_file)
      num_features = 0
      file = File.open(train_file, 'r')
      file.each_line do |line|
        num_features = line.strip.split(" ").length - 1
        num_features += 1 #Adding 1 for the offset feature
        break
      end
      num_features
    end

    def train(train_file, num_features)
      puts "Training: #{train_file} Num_Features:#{num_features}"
      puts "STEP_SIZE: #{@@STEP_SIZE} REGULARIZATION_CONSTANT: #{@@REGULARIZATION_CONSTANT}"
      STDOUT.flush

      # initialize constants
      prng = Random.new(srand())
      prev_theta = Array.new(num_features) { |e| e = prng.rand }
      @theta = Array.new(num_features) { |e| e = prng.rand }
      features = Array.new(num_features, 0.0)
      feature_indices = (0..num_features-1).to_a
      is_converged = false
      start_time = Time.now
      error = 0.0

      iteration = 0
      while iteration < @@MAX_ITERATIONS and not is_converged do
        File.open(train_file, 'r').each do |line|
          iteration += 1
          if iteration > @@MAX_ITERATIONS then break end
          prev_theta = @theta.dup

          # read input from file
          line_contents = line.strip.split(" ")
          target = line_contents[0].to_f
          feature_indices.each do |feature_index|
            features[feature_index] = line_contents[feature_index].to_f
          end
          features[0] = 1.0
     
          # compute predicted value of target and other required terms for
          # performing gradient descent
          dotproduct = 0.0
          feature_indices.each do |feature_index|
            dotproduct += features[feature_index] * @theta[feature_index]
          end
          fhat = Math.tanh(dotproduct)
          weight = -1 * (1 - fhat * fhat) * (target - fhat)
          error += (fhat - target) * (fhat - target);

          # Do Gradient Descent
          norm_diff = 0.0
          feature_indices.each do |feature_index|
            @theta[feature_index] = prev_theta[feature_index] - 
                                    @@STEP_SIZE * (                 
                                      weight * features[feature_index] + 
                      @@REGULARIZATION_CONSTANT * prev_theta[feature_index])

            norm_diff +=(@theta[feature_index] -prev_theta[feature_index]) *
                        (@theta[feature_index] - prev_theta[feature_index])
          end
          norm_diff = Math.sqrt(norm_diff)

          # Check for convergence using either of:
          # 1) norm of (theta - prev_theta) < @@CONVERGENCE_THRESHOLD after 
          #   @@MIN_ITERATIONS
          # 2) we have iterated @@MAX_ITERATIONS times
          if (iteration > @@MIN_ITERATIONS and 
              norm_diff < @@CONVERGENCE_THRESHOLD) or
              iteration > @@MAX_ITERATIONS
            is_converged = true
            break
          end

          # print information for timing and debugging
          if iteration % @@NUM_ITERATIONS_FOR_TIMING == 0
            puts "Time taken to process #{iteration} lines is: #{Time.now - start_time} seconds"
            STDOUT.flush
          end

          # check how are we doing over the validation file
          if iteration % @@NUM_ITERATIONS_FOR_PRINT == 0
            @output_file.write("File:#{train_file} iteration:#{iteration} error:#{Math.sqrt(error / @@NUM_ITERATIONS_FOR_PRINT)}\n")
        
            error = 0.0
            @theta.each do |feature_weight, feature_index| 
              print " #{feature_index}:#{feature_weight}"
            end
            puts " "
            STDOUT.flush

            test(@validation_file, num_features)
          end
        end # end of iterating over file
      end # end of while loop over @@MAX_ITERATIONS


      puts "Convergence after #{iteration} iterations over the training set"
      puts "Time for convergence: #{Time.now - start_time} seconds"
      puts "Feature weights:"
      @theta.each do |feature_weight, feature_index|
        print " #{feature_index}:#{feature_weight}"
      end
      puts " "
      STDOUT.flush
    end

    def test(file, num_features)
      puts "Testing: #{file} Num_Features:#{num_features}"
      STDOUT.flush
      # initialize constants
      features = Array.new(num_features, 0.0)
      feature_indices = (0..num_features-1).to_a
      start_time = Time.now
      error = 0.0

      iteration = 0
      File.open(file, 'r').each do |line|
        iteration += 1

        # read input from file
        line_contents = line.strip.split(" ")
        target = line_contents[0].to_f
        feature_indices.each do |feature_index|
          features[feature_index] = line_contents[feature_index].to_f
        end
        features[0] = 1.0
   
        # compute predicted value of target
        dotproduct = 0.0
        feature_indices.each do |feature_index|
          dotproduct += features[feature_index] * @theta[feature_index]
        end
        fhat = Math.tanh(dotproduct)
        error += (fhat - target) * (fhat - target);

        if iteration % @@NUM_ITERATIONS_FOR_TIMING == 0
          puts "Time taken to process #{iteration} lines is: #{Time.now - start_time} seconds"
          STDOUT.flush
        end

      end # end of file iteration

      puts "Time taken to process #{iteration} lines is: #{Time.now - start_time} seconds"
      @output_file.write("File:#{file} iteration:#{iteration} error:#{Math.sqrt(error / iteration)}\n")
      STDOUT.flush
    end
end


#ranker = Ranker.new({train_file: "sample_train.txt", validation_file: "sample_validation.txt", test_file: "sample_publictest.txt"})
ranker = Ranker.new({train_file: "extraction_train.txt", validation_file: "extraction_validation.txt", test_file: "extraction_publictest.txt"})
ranker.launch!

