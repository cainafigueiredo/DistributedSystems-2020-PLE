Compiling ProducerConsumerSem.c:
  - Use the following command: "gcc -pthread ProducerConsumerSem.c -lm -o ProducerConsumerSem"
  
Running ProducerConsumerSem:
  - Use the following command: "./ProducerConsumerSem -np X -nc Y -n Z -m W", 
    where X, Y, Z and W should be replaced by integers.
  
  - Parameters: 
      - np: The number of producer threads;
      - nc: The number of consumer threads;
      - n : The capacity of shared memory;
      - m : Total of numbers that should be consumed before the program ends.
      
  - Results:
      - Terminal prints: The results of primality tests;
      - elapsedTime.out: A file at the same directory as the program is. It contains the elapsed time sinces threads creation until the program ends.
      
Running case studies:
  - Use the following command: "./CaseStudies.sh". 
    It will call ProducerConsumerSem 10 times for each parameters combination and will get the mean running time. Hence, it stores both the parameters
    and mean running time in a CSV file at the same directory. This CSV was named out.csv. It can be used to plot a graph, for example.
