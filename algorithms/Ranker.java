import java.util.HashMap;
import java.util.Random;
import java.io.BufferedReader;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.File;
import java.io.IOException;

public class Ranker {
  private static final int MIN_ITERATIONS = 80000000;
  private static final int MAX_ITERATIONS = 100000000;
  private static final int NUM_ITERATIONS_FOR_TIMING = 1000000;
  private static final int NUM_ITERATIONS_FOR_PRINT = 5000000;
  private static final double CONVERGENCE_THRESHOLD = 1e-4;
  private static final double STEP_SIZE = 1e-8;
  private static final double REGULARIZATION_CONSTANT = 1e-3;

  private String trainingFile = "";
  private String validationFile = "";
  private String testingFile = "";
  private String outputFile = "";
  private int numFeatures = 0;
  private double[] theta;
  private FileWriter fileWriter = null;

  public Ranker(HashMap<String, String> options) throws IOException {
    trainingFile = options.get("train_file");
    validationFile = options.get("validation_file");
    testingFile = options.get("test_file");
    outputFile = options.get("output_file");
    String numFeaturesString = options.get("num_features");
    if (numFeaturesString == null) 
      numFeatures = getNumFeatures(trainingFile);
    else 
      numFeatures = Integer.parseInt(numFeaturesString);

    this.theta = new double[numFeatures];
    Random rand = new Random();
    for(int i = 0; i < numFeatures; ++i) {
      this.theta[i] = rand.nextDouble();
    }
  }

  public void launch() throws IOException {
    try {
      this.fileWriter = new FileWriter(this.outputFile);

      train(this.trainingFile);

      test(this.trainingFile);
      test(this.validationFile);
      test(this.testingFile);    
    } finally {
      if (this.fileWriter != null) {
        this.fileWriter.close();
      }
    }
  }

  private int getNumFeatures(String file) throws IOException {
    int numFeaturesInFile = 0;
    BufferedReader br = null;
    try {
      br = new BufferedReader(new FileReader(new File(file)));
      String line = null;
      while( (line = br.readLine()) != null) {
        numFeaturesInFile = line.trim().split(" ").length - 1;
        numFeaturesInFile += 1; // for offset feature
        break;
      }
    } finally {
      if (br != null) br.close();
    }

    return numFeaturesInFile;
  }

  private void train(String file) throws IOException {
    System.out.println(String.format("Training: %s", file));
    System.out.println(String.format("NumFeatures:%d StepSize:%6.3e RegularizationConstant:%6.3e", this.numFeatures, Ranker.STEP_SIZE, Ranker.REGULARIZATION_CONSTANT ));
    System.out.flush();

    String[] lineContents = new String[numFeatures];
    double[] features = new double[numFeatures];
    double[] prevtheta = new double[numFeatures];
    double error = 0.0;
    boolean isConverged = false;
    long startTime = System.currentTimeMillis();
    int iteration = 0;

    while( iteration < Ranker.MAX_ITERATIONS && !isConverged ) {
      BufferedReader br = null;
      try {
        br = new BufferedReader(new FileReader(new File(file)));
        String line = null;
        while( (line = br.readLine()) != null) {
          // Copy over values from previous iteration
          iteration++;
          for (int featureIndex = 0; featureIndex < numFeatures; ++featureIndex) {
            prevtheta[featureIndex] = theta[featureIndex];
          }          

          // Read the feature values from current line
          lineContents = line.trim().split(" ");
          double target = Double.parseDouble(lineContents[0]);
          for (int featureIndex = 0; featureIndex < numFeatures; ++featureIndex) {
            features[featureIndex] = Double.parseDouble(lineContents[featureIndex]);
          }
          features[0] = 1.0;

          // compute predicted value of target and other required terms for
          // performing gradient descent
          double dotproduct = 0.0;
          for (int featureIndex = 0; featureIndex < numFeatures; ++featureIndex) {
            dotproduct += features[featureIndex] * theta[featureIndex];
          }
          double fhat = Math.tanh(dotproduct);
          double weight = -1 * (1 - fhat * fhat) * (target - fhat);
          error += (fhat - target) * (fhat - target);

          // Do Gradient Descent
          double norm_diff = 0.0;
          for (int featureIndex = 0; featureIndex < numFeatures; ++featureIndex) {
            theta[featureIndex] = prevtheta[featureIndex] - 
                                    Ranker.STEP_SIZE * (
                                      weight * features[featureIndex] +
                   Ranker.REGULARIZATION_CONSTANT * prevtheta[featureIndex]
                                  );
            norm_diff += (theta[featureIndex] - prevtheta[featureIndex]) *
                          (theta[featureIndex] - prevtheta[featureIndex]);
          } 
          norm_diff = Math.sqrt(norm_diff);
        
          // Check for convergence using:
          // 1. norm of (theta - prevtheta) < CONVERGENCE_THRESHOLD after
          //      we have iterated over MIN_ITERATIONS
          // 2. we have iterated more than MAX_ITERATIONS
          if ((iteration > Ranker.MIN_ITERATIONS && 
                norm_diff < Ranker.CONVERGENCE_THRESHOLD ) || 
              (iteration > Ranker.MAX_ITERATIONS)) {
            isConverged = true;
            break;
          }

          // print information for timing and debugging
          if (iteration % Ranker.NUM_ITERATIONS_FOR_TIMING == 0) {
            System.out.println(String.format("Time taken to process %d lines is:%6.3e seconds", iteration, (double)(System.currentTimeMillis() - startTime) / (double)1000 ));
            System.out.flush();
          } 

          // check how are we doing over the validation file
          if (iteration % Ranker.NUM_ITERATIONS_FOR_PRINT == 0) {
            fileWriter.write(String.format("File:%s iteration:%d error:%6.3e\n", file, iteration, Math.sqrt(error / Ranker.NUM_ITERATIONS_FOR_PRINT)));

            error = 0.0;
            for (int featureIndex = 0; featureIndex < numFeatures; ++featureIndex) {
              System.out.print(String.format(" %d %6.3e", featureIndex, theta[featureIndex]));
            }
            System.out.println(" ");
            System.out.flush();

            test(this.validationFile);
          }

 
        } // end of iterating over the file
      } finally {
        if (br != null) br.close();
      }
      
    } // end of looping over MAX_ITERATIONS

    System.out.println(String.format("Convergence after %d iterations over the training set", iteration));
    System.out.println(String.format("Time taken for convergence: %6.3e seconds", ((double)(System.currentTimeMillis() - startTime) / (double)1000) ));
    System.out.println("Feature Weights:");
    for (int featureIndex = 0; featureIndex < numFeatures; ++featureIndex) {
      System.out.print(String.format(" %d %6.3e", featureIndex, theta[featureIndex]));
    }
    System.out.println(" ");
    System.out.flush();
  }

  private void test(String file) throws IOException {
    System.out.println(String.format("Testing: %s", file));
    System.out.flush();

    String[] lineContents = new String[numFeatures];
    double[] features = new double[numFeatures];
    double error = 0.0;
    long startTime = System.currentTimeMillis();
    int iteration = 0;

    BufferedReader br = null;
    try {
      br = new BufferedReader(new FileReader(new File(file)));
      String line = null;
      while( (line = br.readLine()) != null) {
        iteration++;

        // Read the feature values from current line
        lineContents = line.trim().split(" ");
        double target = Double.parseDouble(lineContents[0]);
        for (int featureIndex = 0; featureIndex < numFeatures; ++featureIndex) {
          features[featureIndex] = Double.parseDouble(lineContents[featureIndex]);
        }
        features[0] = 1.0;

        // compute predicted value of target
        double dotproduct = 0.0;
        for (int featureIndex = 0; featureIndex < numFeatures; ++featureIndex) {
          dotproduct += features[featureIndex] * theta[featureIndex];
        }
        double fhat = Math.tanh(dotproduct);
        error += (fhat - target) * (fhat - target);

        // print information for timing and debugging
        if (iteration % Ranker.NUM_ITERATIONS_FOR_TIMING == 0) {
          System.out.println(String.format("Time taken to process %d lines is:%6.3e seconds", iteration, (double)(System.currentTimeMillis() - startTime) / (double)1000 ));
          System.out.flush();
        } 

      } // end of iterating over the file
    } finally {
      if (br != null) br.close();
    }

    System.out.println(String.format("Time taken to process %d lines is:%6.3e seconds", iteration, (double)(System.currentTimeMillis() - startTime) / (double)1000 ));
    fileWriter.write(String.format("File:%s iteration:%d error:%6.3e\n", file, iteration, Math.sqrt(error / Ranker.NUM_ITERATIONS_FOR_PRINT)));
    System.out.flush();
    
  }
}
