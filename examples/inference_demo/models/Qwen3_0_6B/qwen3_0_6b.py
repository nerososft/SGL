from transformers import pipeline, AutoTokenizer, AutoModelForCausalLM

# 加载本地模型（确保模型文件在当前目录或指定路径）
model_path = "./"  # 替换为实际模型路径

# 方法2: 手动加载模型和分词器
tokenizer = AutoTokenizer.from_pretrained(model_path)
model = AutoModelForCausalLM.from_pretrained(model_path)

inputs = tokenizer("What is the capital of France?", return_tensors="pt")
outputs = model.generate(**inputs, max_length=256)
generated_text = tokenizer.decode(outputs[0], skip_special_tokens=True)
print("Manual Model Output:", generated_text)