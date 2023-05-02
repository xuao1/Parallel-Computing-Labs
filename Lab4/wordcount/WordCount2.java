import java.io.IOException;
import java.util.StringTokenizer;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.util.GenericOptionsParser;

public class WordCount2 {

  // 定义一个名为TokenizerMapper的静态内部类，继承自Mapper类，
  // 指定输入键、输入值、输出键和输出值的类型为Object、Text、Text和IntWritable。
  public static class TokenizerMapper 
       extends Mapper<Object, Text, Text, IntWritable>{

    // 定义一个值为1的IntWritable类型的常量one，用于表示单词的出现次数。
    private final static IntWritable one = new IntWritable(1);
    // 定义一个Text类型的变量wordLength，用于保存单词的长度。
    private Text wordLength = new Text();
    // 定义map方法  
    public void map(Object key, Text value, Context context
                    ) throws IOException, InterruptedException {
      // 使用StringTokenizer分词器对输入的文本进行分词。
      StringTokenizer itr = new StringTokenizer(value.toString());
      while (itr.hasMoreTokens()) { 
        String word = itr.nextToken(); 
        // 将单词的长度存入wordLength中
        wordLength.set(Integer.toString(word.length()));
        // 通过Context的write方法将键值对传递给Reducer
        context.write(wordLength, one);
      }
    }
  }
  
  // 定义一个名为IntSumReducer的静态内部类，继承自Reducer类，
  // 指定输入键、输入值、输出键和输出值的类型为Text和IntWritable。
  public static class IntSumReducer 
       extends Reducer<Text,IntWritable,Text,IntWritable> {
    // 定义一个IntWritable类型的变量result，用于保存统计结果
    private IntWritable result = new IntWritable();

    public void reduce(Text key, Iterable<IntWritable> values, 
                       Context context
                       ) throws IOException, InterruptedException {
      int sum = 0; // 出现次数的累加结果
      for (IntWritable val : values) {
        sum += val.get(); 
      }
      result.set(sum);
      // 通过Context的write方法将键值对传递给输出。
      context.write(key, result);
    }
  }

  public static void main(String[] args) throws Exception {
    Configuration conf = new Configuration();
    String[] otherArgs = new GenericOptionsParser(conf, args).getRemainingArgs();
    if (otherArgs.length != 2) {
      System.err.println("Usage: wordcount <in> <out>");
      System.exit(2);
    }
    // 创建一个Job对象job
    Job job = new Job(conf, "word length count");
    // 设置WordCount2类作为Job的运行类
    job.setJarByClass(WordCount2.class);
    // 设置TokenizerMapper类作为Job的Mapper
    job.setMapperClass(TokenizerMapper.class);
    // 设置IntSumReducer类作为Job的Combiner
    job.setCombinerClass(IntSumReducer.class);
    // 设置IntSumReducer类作为Job的Reducer
    job.setReducerClass(IntSumReducer.class);
    // 设置输出的键的类型为Text。
    job.setOutputKeyClass(Text.class);
    // 设置输出的值的类型为IntWritable。
    job.setOutputValueClass(IntWritable.class);
    FileInputFormat.addInputPath(job, new Path(otherArgs[0]));
    FileOutputFormat.setOutputPath(job, new Path(otherArgs[1]));
    System.exit(job.waitForCompletion(true) ? 0 : 1);
  }
}
